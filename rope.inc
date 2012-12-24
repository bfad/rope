#! /usr/bin/env lasso9

define usage => {
    local(cmd) = $argv->get(1)->lastComponent
    stdoutnl("\
usage: " + #cmd + " new PROJECT_NAME\n
       " + #cmd + " server [-H <ip address>] [-P <port>] [-R <webroot path>] 
       " + (' ' * #cmd->size) + "        [-s <\"space separated lasso source suffixes\">] 
       " + (' ' * #cmd->size) + "        [<project path>]"
    )
    sys_exit(-1)
}

define path_master_home => (sys_getEnv(`LASSO9_MASTER_HOME`) || '/var/lasso/home') + '/'

define run_new => {
    $argc < 3 ? usage
    
    if(not file(path_master_home + 'LassoApps/rope.lassoapp')->exists
        and not file(path_master_home + 'LassoApps-disabled/rope.lassoapp')->exists
    ) => {
        stdoutnl("ERROR: Could not find rope.lassoapp in " + path_master_home + " directory.")
        sys_exit(-1)
    }
    
    local(path_project) = string($argv->get(3))
    #path_project->removeTrailing('/')
    
    local(project_name) = #path_project->lastcomponent
    #project_name->replace('_', ' ')&titlecase
    
    dir(#path_project)->create&setcwd
    dir('webapp/')    ->create
    dir('webroot/')   ->create
    dir('views/')     ->create
    
    file('webapp/_config.inc')->doWithClose => {
        #1->writeString("\
            not rope->configLoaded?\n\
            rope->config(\n    \
                `app-name` = `" + #project_name + "`,\n\
            )"
        )
    }
    
    dir('lasso_home/')   ->create&setcwd
    dir('JavaLibraries/')->create
    dir('JDBCDrivers/')  ->create
    dir('LassoApps/')    ->create
    dir('LassoModules/') ->create
    dir('SQLiteDBs/')    ->create
    
    io_file_symlink('../webapp' , 'LassoStartup')
    local(f_ropeapp) = file(path_master_home + 'LassoApps-disabled/rope.lassoapp')
    #f_ropeapp->exists
        ? #f_ropeapp->copyTo('LassoApps/' + #f_ropeapp->name)
}

define run_server => {
    // Default Values
    local(httpAddr)   = '0.0.0.0'
    local(httpPort)   = 8000
    local(httpRoot)   = '/' + io_file_getcwd + '/webroot/'
    local(scriptExts) = (:'lasso','inc')
    local(project_path) = io_file_getcwd
    
    
    define dir_import(d::dir, ext::staticarray=(:'lasso', 'inc')) => {
        with f in #d->eachFile
        where #ext->contains(#f->path->split('.')->last) 
        do file_import(#f)
    
        with f in #d->eachDir do dir_import(#f)
    }
    define file_import(f::file) => {
        sourcefile(#f->readString, #f->path, false, false)->invoke
    }


    define rope_connection_handler => type { parent http_server_connection_handler
        public onCreate(root::string, exts::trait_forEach) => ..onCreate(#root, #exts)
    
        public invoke(con::http_server_web_connection) => {
            protect => {
                handle_failure => {
                    stdoutnl(error_msg)
                    stdoutnl(error_stack)
                }

                web_request  = web_request_impl(#con) // make it available for the thread
                web_response = web_response_impl(#con)

                local(path)     = #con->requestParams->get(::PATH_INFO)->asString
                local(ext)      = #path->splitExtension->second
                local(document) = file(.documentRoot + #path)
                //local(is_lasso) = (#path->beginsWith(http_server_apps_path) or not #document->exists or (#ext->size and .scriptExtensions >> #ext))
                local(is_lasso) = (#path->beginsWith(`/lasso9/`) or (#ext->size and .scriptExtensions >> #ext))
            
                if(not #is_lasso and #document->exists and not #document->isDir) => {
                    web_response->setStatus(200, 'OK')
                    .sendFile(#document, #ext)
                // pass off non-existing files to rope
                else(not #is_lasso)
                    #is_lasso = true
                    #con->requestParams->insert(::LASSOSERVER_APP_PREFIX=`/lasso9/rope`)
                    #con->requestParams->insert(::PATH_INFO=`/rope/url_handler.lasso`)
                }
                if(#is_lasso) => {
                    #con->requestParams->insert(::LASSOSERVER_DOCUMENT_ROOT=.documentRoot)
                    web_response->respond
                }
            }
            // keep alive requests use the same thread, so reset thread-local data
            web_request = null
            web_response = null
            document = null
            var(_lassoapp_include_stack_) = null
            var(__HTML_REPLY__) = ''
        }
    }
    // This allows me to access /lasso9/admin without errors
    define lassopro_system_status => thread {
        data private start::integer = date->asInteger
        public uptime => date->asInteger - .start
    }


    // Parse any command-line arguments
    local(opts) = $argv->asArray
    #opts->remove(1,2)
    while(#opts->size > 0) => {
        if(#opts->size == 1) => {
            #project_path = #opts(1)
            #opts->remove
            loop_abort
        }
        match(bytes(#opts(1))) => {
        case(bytes(`-H`))
            #httpAddr = #opts(2)
            '127.0.0.1' == #httpAddr
                ? #httpAddr = '0.0.0.0'
        case(bytes(`-P`))
            #httpPort = integer(#opts(2))
        case(bytes(`-R`))
            #httpRoot = #opts(2)
        case(bytes(`-s`))
            #scriptExts = #opts(2)->split(' ')
        case
            usage
        }
        #opts->remove(1,2)
    }
    not dir(#project_path)->exists or not dir(#project_path + '/lasso_home/')->exists
        ? fail('The specified project path does not exist or is not a valid rope project')
        
    if(not file(path_master_home + 'LassoApps/rope.lassoapp')->exists
        and not file(#project_path + '/lasso_home/LassoApps/rope.lassoapp')->exists
    ) => {
        stdoutnl("ERROR: Could not find rope.lassoapp in " + path_master_home + " directory.")
        sys_exit(-1)
    }

    // Set everything up
    /*
    When the Lasso interpreter starts, if no LASSO9_HOME env variable has been defined, then
    it defaults to /var/lasso/home (for OS X & Linux). If no LASSO9_MASTER_HOME is specified,
    then it doesn't define it - just leaves it blank as the home is the most important thing.

    If you have specified a home, you better have either specified a master home or that home
    better contain everything (such as Lasso Admin). Typically, instance manager specifies
    the default home path (/var/lasso/home) as the LASSO9_MASTER_HOME and allows you to
    create multiple instance with their own LASSO9_HOME's. This allows every instance to
    share the code in the LASSO9_MASTER_HOME so that each instance doesn't need its own copy
    of the Lasso Admin code or its own copy of the Lasso core libraries, etc.

    For the Rope server, both LASSO9_HOME and LASSO9_MASTER_HOME will always be set. Users
    can specify their own LASSO9_MASTER_HOME env variable or the default of /var/lasso/home
    will be used. The default LASSO9_HOME is the curreent working directory, but users can
    specify a different home using command-line options. (Any user-set LASSO9_HOME env 
    variable will be ignored and overridden.)
    */
    sys_setEnv('LASSO9_MASTER_HOME', path_master_home)
    sys_setEnv('LASSO9_HOME'       , #project_path + '/lasso_home/')

    protect => {
        handle_error => {
            stdoutnl(error_code + ': ' + error_msg + '\n' + error_stack)
        }
        database_initialize
         session_initialize
        lassoapp_initialize
           admin_initialize
             log_initialize
           email_initialize
           ljapi_initialize
        security_initialize

        stdoutnl('Loading Startup Items')
        dir_import(dir(sys_userStartupPath), #scriptExts)

        stdoutnl('Starting webserver on ' + #httpAddr + ':' + #httpPort)
        http_server->start(#httpPort, #httpAddr, -handler=rope_connection_handler(#httpRoot, #scriptExts))
    }
}

1 == $argv->size
    ? usage
match($argv->get(2)) => {
case('new', 'n')
    run_new
case('server', 's')
    run_server
case
    usage
}