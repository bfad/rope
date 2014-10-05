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

define passThruStdoutStderr(proc::sys_process) => {
    local(did_read) = false
    // Until there's nothing there, keep reading from the child process
    // and writing it to the parent process
    local(stdout, stderr) = (:#proc->read,#proc->readError)
    while(#stdout->size or #stderr->size) => {
        #did_read = true
        #stdout
            ? file_stdout->writeBytes(#stdout)
        #stderr
            ? file_stderr->writeBytes(#stderr)
        local(stdout, stderr) = (:#proc->read,#proc->readError)
    }

    return #did_read
}

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
    dir('LassoStartup/') -> create
    
    io_file_symlink('../webapp' , 'rope_webapp')
    file('LassoStartup/_load_rope_webapp.lasso')->doWithClose => {
        #1->openTruncate&writeString(`dir_import(dir(sys_homePath + '/rope_webapp/'))`)
    }

    local(f_ropeapp) = file(path_master_home + 'LassoApps-disabled/rope.lassoapp')
    #f_ropeapp->exists
        ? #f_ropeapp->copyTo('LassoApps/' + #f_ropeapp->name)
}

define run_server => {
    // Default Values
    local(httpAddr)    = '0.0.0.0'
    local(httpPort)    = 8000
    local(httpRoot)    = '/' + io_file_getcwd + '/webroot/'
    local(scriptExts)  = (:'lasso','inc')
    local(projectPath) = io_file_getcwd
    local(lassoHome)   = sys_getEnv(`LASSO9_HOME`)->asString

    #lassoHome->removeTrailing('/')
    staticarray('', '/var/lasso/home', '/var/lasso/global_cli_home') >> #lassoHome
        ? #lassoHome = null

    // Parse any command-line arguments
    local(opts) = $argv->asArray
    #opts->remove(1,2)
    while(#opts->size > 0) => {
        if(#opts->size == 1) => {
            #projectPath = #opts(1)
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

    #lassoHome = (#lassoHome || #projectPath + '/lasso_home') + '/'
    not dir(#lassoHome)->exists
        ? fail('Could not find the Lasso home directory')
        
    if(not file(path_master_home + 'LassoApps/rope.lassoapp')->exists
        and not file(#lassoHome  + 'LassoApps/rope.lassoapp')->exists
    ) => {
        stdoutnl("ERROR: Could not find rope.lassoapp in " + path_master_home + " or " + #lassoHome)
        sys_exit(-1)
    }

    local(proc) = sys_process
    handle => { #proc->close }
    #proc->open(
        '/usr/sbin/lassoserver',
        (:'-httproot', #httpRoot, '-scriptextensions', #scriptExts->join(';'), '-addr', #httpAddr, '-p', #httpPort),
        (:'LASSO9_MASTER_HOME=' + path_master_home, 'LASSO9_HOME=' + #lassoHome)
    )
    local(sleep) = 0
    while(#proc->isOpen) => {
        passThruStdoutStderr(#proc)
            ? #sleep  = 0
            | #sleep += 1
        #sleep > 1000
            ? #sleep = 1000
        sleep(#sleep)
    }
    passThruStdoutStderr(#proc)
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