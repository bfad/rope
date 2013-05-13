/**!
    A thread object that tracks configuration settings, routes, and controllers. 
    When not in produciton mode it also tracks files in LassoStartup. Due to the
    nature of thread objects, it can't reload changed files, but relies on the
    main thread to do the actual reloading.
    
    This thread object is what Rope uses internally, and is not meant to be 
    accessed by a user directly. All interaction with the framework is meant to go
    through the ``rope`` method.
*/
define rope_thread => thread {
    data private startupFiles::map = map,
         private controllers ::map = map,
         private routes      ::map = map,
         private configLoaded::boolean = false,
         private config      ::map = map(
                `app-name`         = pair(`UnNamed`,::string),
                `sessions-enabled` = pair(true, ::boolean),
                `sessions-expires` = pair(1440, ::integer)
            )

    public controllers  => .`controllers`
    public routes       => .`routes`
    public configLoaded => .`configLoaded`
    /**!
        When not in a poduction environment, Rope grabs the current modification times
        for all files in LassoStartup so that each time ``rope->invoke`` is called
        (every request), it will reload any files whose modification time has changed.
    */
    public onCreate => {
        if(`production` != sys_getEnv(`ROPE_ENV`)) => {
            with path in dir(sys_userStartupPath)->eachFilePathRecursive 
            do .`startupFiles`->insert(#path=file(#path)->modificationTime)
        }
    }
    /**!
        Due to the nature of thread objects and that the files that need reloading
        could call methods that try to access ``rope_thread``, the thread object can't
        reload changed files, but it can return an array of files for the main thread
        to reload.
        
        While looking for files to reload, this method also sees if any files have
        been deleted and removes them from being tracked by Rope.
    */
    public findFilesToReload => {
        `production` == sys_getEnv(`ROPE_ENV`)
            ? return

        local(cur_files_tracking) = .startupFiles->keys->asArray
        local(files_to_reload)    = array

        with path in dir(sys_userStartupPath)->eachFilePathRecursive
        let f = file(#path) do {
            .startupFiles->contains(#path)
                ? #cur_files_tracking->removeAll(#path)
            if(not .startupFiles->contains(#path) or #f->modificationTime > .startupFiles->get(#path)) => {
                .`startupFiles`->insert(#path=#f->modificationTime)
                #files_to_reload->insert(#path)
            }
        }
        with path in #cur_files_tracking do .`startupFiles`->remove(#path)

        return #files_to_reload
    }

    public controller(key::string)      => .`controllers`->get(#key)
    public controllerExist(key::string) => .`controllers`->contains(#key)
    public insertController(item::pair) => {
        .`controllers`->insert(#item)
    }
    public removeController(key::string) => {
        .`controllers`->remove(#key)
        
        with item in .'routes'->eachPair do {
            local(routes) = #item->second
            local(index)  = 0
            while(#routes->size > #index++) => {
                #routes->get(#index)->second == #key
                    ? #routes->remove(#index--) // Removing an item means we need to recheck current position
            }
        }
    }
    public removeAllControllers => {
        .'controllers' = map
        .`routes`      = map
    }
    
    
    public insertRoute(item::pair) => {
        local(path_size) = #item->first->size
        not .`routes`->contains(#path_size)
            ? .`routes`->insert(#path_size=array)

        not .`routes`->get(#path_size)->contains(#item)
            ? .`routes`->get(#path_size)->insert(#item)
    }
    public findRoutesSized(key::integer) => .`routes`->find(#key)
    
    public config(key::string) => .`config`->find(#key)->first
    /**!
        This method expects an argument list of pairs whose first value is the name of
        the configuration being set and the second is the value to set it to. The list
        of configurations and their defaults is stored in a private ``config`` type
        variable. Errors will be thrown if the configuration doesn't exist or if the
        value passed for the configuration is not of the correct type.
    */
    public config(...) => {
        .configLoaded? fail(`Configuration already loaded`)
        
        with el in #rest
        where #el->isA(::pair)
        do {
            not #el->second->isA(.`config`->get(#el->first)->second)
                ? fail(error_code_invalidParameter, `The "` + #el->first + `" config option requires a ` + string(.`config`->get(#el->first)->second))
            .`config`->get(#el->first)->first = #el->second
        }
        
        .`configLoaded` = true
    }
}
::rope_thread->doccomment = tag('rope_thread_thread$')->doccomment
define rope_impl => type {
    data public urlParams::array = array,
        private status_msg
    
    /**!
        This is where the magic all happens. Every web request that rope handles runs
        this method and follows these steps:

            #. If we are not running in production mode, it checks for any new files in
               LassoStartup or any modified existing files and loads them. 

            #. It matches the URL path to a route. If there are any URL parameters in 
               the path then it also loads them up.

            #. It continues the Rope Session as long as sessions aren't turned off.

            #. It executes the controller block that matches the route that was found.
    */
    public invoke => {
        `production` != .env
            ? .startupCheckNLoad

        local(path)  = .path->asCopy->remove(1,1)&split('/')
        local(route) = .matchRoute(#path)
        
        //Setup URL parameters
        local(i) = #path->size
        while(#i--) => {
            local(part) = #route->first->get(loop_count)
            
            `:` == #part->get(1)
                ? .'urlParams'->insert(#part->sub(2)=#path->get(loop_count))
        }
        .continueSession
        return .controller(#route->second)->invoke
    }
    public startupCheckNLoad => {
        with path in rope_thread->findFilesToReload do sourcefile(file(`/` + #path)->readString, #path, false, false)->invoke
    }
    
    public config(...)  => rope_thread->config(:#rest)
    public configLoaded => rope_thread->configLoaded
    
    /**!
        This method serves two functions: 1) It registers a controller and its code 
        block into Rope. 2) It registers routes with an existing controller.

        If a controller of a given name doesn't exist, then this method expects a block
        to be passed in for it to register the controller. You can register routes at
        the same time you are registering the controller with its code block. If you do
        not pass a code block, then it is assumed that you are registering routes to an
        existing controller and an error will be thrown if that controller does not
        exist.

        Examples::

            // Good
            rope->register('products') => {
                .... snip ....
            }
            rope->register('product-show', -routes=(:'/product/:id')) => {
                .... snip ....
            }
            rope->register('product-show', -routes=(:'/widget/:id'))

            // Bad
            rope->register('new-controller', -routes=(:'/moose/hair/rhino'))
    */
    public register(controller::string, -routes::staticarray=(:)) => {
        givenBlock? rope_thread->insertController(#controller=givenBlock)
        
        not givenBlock and #routes->size == 0
            ? fail(`This method requires you to either also pass routes or a givenBlock`)
    
        not rope_thread->controllerExist(#controller)
            ? fail(error_code_invalidParameter, `The specified controller does not exist`)
        // Not sure why, but while beats the socks off loop and query expressions in performance
        local(i) = #routes->size
        while(#i--) => {
            not #routes->get(loop_count)->isA(::string)
                ? fail(error_code_invalidParameter, `Invalid parameter: One of the routes passed was not a string`)
            
            local(path) = #routes->get(loop_count)
            #path->get(1) == `/`
                ? #path->remove(1,1)
            #path->last == `/`
                ? #path->remove(#path->size, 1)
            
            rope_thread->insertRoute(#path->split(`/`)=#controller)
        }
    }
    private absolutePath(path::string) => {
        #path->sub(1,2) == '//'
            ? return #path

        #path->get(1) != '/'
            ? #path = io_file_getcwd + '/' + #path
        
        return '/' + #path
    }

    public deregister(controller::string) => {
        rope_thread->removeController(#controller)
    }
    public deregisterAll => rope_thread->removeAllControllers
    
    /**!
        This method can take either a string that's the URL path or an array, each
        element of which is part of the path. The algorithm for matching routes is a bit
        complex:

            #. The Route must be the same number of parts as the path.

            #. Routes can have wild-card parts, indicated by the part starting with a
               colon. (These are matched as URL parameters later.) However, the matcher
               prefers those routes whose earliest parts match non-wildcard parts. This
               means that for two routes "/prada/:id" and "/:designer/shoes", the URL 
               "/prada/shoes" will match the first - "/prada/:id".

            #. All the parts must be found to match either exactly or a wildcard.

            #. It is an error if no routes are found or if more than one route matches.
    */
    public matchRoute(path::string) => .matchRoute(#path->asCopy->remove(1,1)&split('/'))
    private matchRoute(path::array) => {
        local(i)     = #path->size
        local(found) = rope_thread->findRoutesSized(#i)
        local(good, better, ii)
        
        while(#i-- and #ii := #found->size) => {
            local(pi) = loop_count
            #good     = array
            #better   = array
            
            while(#ii--) => {
                #found->get(loop_count)->first->get(#pi) == #path->get(#pi)
                    ? #better->insert(#found->get(loop_count))
                #found->get(loop_count)->first->get(#pi)->first == ':'
                    ? #good->insert(#found->get(loop_count))
            }
            #better->size
                ? #found = #better
                | #found = #good
        }
        #found->size == 0 and .controllerExist(`rope-fallback`)
            ? return pair(#path=`rope-fallback`)
        #found->size == 0
            ? fail(error_code_fileNotFound, `No matching route found`)
        #found->size > 1
            ? fail(`Multiple routes matched the given path`)
            
        return #found->first
    }
    
    
    public controller     (key::string) => rope_thread->controller(#key)
    public controllerExist(key::string) => rope_thread->controllerExist(#key)

    public routesSized(size::integer) => rope_thread->findRoutesSized(#size)
    
    /**!
        This method returns "dev" if the ROPE_ENV environment variable has not been set.
        Otherwise it returns the value stored in the environment variable, even if that
        value is an empty string
    */
    public env => (sys_getEnv('ROPE_ENV')->isA(::void) ? 'dev' | sys_getEnv('ROPE_ENV'))
    
    
    public protocol => (web_request->isHttps ? 'https://' | 'http://')
    public method   => .param(`_method`) || web_request->requestMethod
    public hostname => web_request->httpHost
    public path     => {
        local(path) = web_request->requestUri->split('?')->get(1)->split('/')
        #path->last->contains('.')
            ? #path->get(#path->size) = #path->last->split('.')->remove&join('.')
        
        #path->size > 2 and #path->last->isEmpty ? #path->remove
        return #path->join('/')
    }
    public extension => {
        local(ext) = web_request->requestUri->split('?')->get(1)->split('/')->last->split('.')
        #ext->size > 1
            ? return #ext->last
        return 'html'
    }
    

    public queryParams => web_request->queryParams
    public postParams  => web_request->postParams
    public params      => tie(.urlParams, tie(.queryParams, .postParams))->asStaticArray
    
    public urlParam  (key::string, -as::tag=::stringOrNull, -default::any=void, -selectAll::boolean=false) => .getParam('urlParams'  , #key, #as, #default, #selectAll)
    public queryParam(key::string, -as::tag=::stringOrNull, -default::any=void, -selectAll::boolean=false) => .getParam('queryParams', #key, #as, #default, #selectAll)
    public postParam (key::string, -as::tag=::stringOrNull, -default::any=void, -selectAll::boolean=false) => .getParam('postParams' , #key, #as, #default, #selectAll)
    public param     (key::string, -as::tag=::stringOrNull, -default::any=void, -selectAll::boolean=false) => .getParam('params'     , #key, #as, #default, #selectAll)
    
    private getParam(type::string, key::string, as::tag, default::any, selectAll::boolean) => {
        if(#selectAll) => {
            return (
                with param in self->\(tag(#type))->invoke
                where #param->first == #key
                select \#as(#param->second)
            )->asStaticArray
        }
        
        with param in self->\(tag(#type))->invoke
        where #param->first == #key
        do { return \#as(#param->second) }
        
        return #default
    }
    
    
    public statusMsg(-status::boolean=false, -message::boolean=false) => {
        fail_if(#status && #message, error_code_invalidParameter, `You can't specify both the status and the message`)
          
        null === .`status_msg`
            ? return null
        
        #status
            ? return .`status_msg`->find(`status`)
        #message
            ? return .`status_msg`->find(`message`)

        return .`status_msg`
    }
    
    public statusMsg(stat_msg::pair)            => { .`status_msg` = map(`status`=#stat_msg->first, `message`=#stat_msg->second) }
    public statusMsg(stat::string, msg::string) => { .`status_msg` = map(`status`=#stat           , `message`=#msg             ) }
    
    
    public continueSession => {
        not .config(`sessions-enabled`)? return
        
        session_start(`RopeSession`, -expires=.config(`sessions-expires`), -useCookie)
        if(session_result != "load") => {
            session_addVar(-name='RopeSession', '_sess_rope')
            session_addVar(-name='RopeSession', '_sess_user')
            var(_sess_rope) = map
            var(_sess_user)
        }
    }
    public sess=(rhs::trait_serializable, key::string) => {
        not .config(`sessions-enabled`)
            ? fail(`Sessions are disabled`)
            
        $_sess_rope->insert(#key=#rhs)
    }
    public sess(key::string) => {
        not .config(`sessions-enabled`)
            ? fail(`Sessions are disabled`)
        
        return $_sess_rope->find(#key)
    }
    public user => {
        not .config(`sessions-enabled`)
            ? fail(`Sessions are disabled`)
        return $_sess_user
    }
    public user=(rhs::trait_rope_user) => {
        not .config(`sessions-enabled`)
            ? fail(`Sessions are disabled`)
        $_sess_user = #rhs
    }
    
    
    private viewForPath(path::string)::file => {
        `/` == #path->get(1)
            ? return file(file_forceRoot('/../views/' + #path))
        
        return file(file_forceRoot(currentCapture->continuation->continuation->callsite_file->stripLastComponent + '/' + #path))
    }
    
    public render(path::string, ...) => { content_body = .include(.viewForPath(#path), #rest) }
    public render(view::file  , ...) => { content_body = .include(#view, #rest) }
    
    /**!
        The ``include`` member method accepts either a file or a string which is a path
        to a file in the view folder. The string can be a relative path if this method
        is being called in a view itself and the file will be assumed to be relative to
        the current file. If passed an absolute path - a path starting with a single 
        slash (/) - it will be assumed that the path is actually relative from the view
        folder itself. A fully-qualified absolute path - a path that starts with two or
        more slashes - will be treated like an absolute path from the root of the file
        system.

        Optionally, this method can take any number of keyword parameters. These
        parameters are setup as local variables for the file they are including. The
        name of the local variable is the parameter name and the value for the variable
        is the corresponding parameter value. Essentially, this allows you to easily
        "pass" local variables for use inside your views.
    */
    public include(path::string, ...) => .include(.viewForPath(#path), #rest)
    public include(view::file, ...)   => .include(#view, #rest)
    public include(view::file, params::staticarray) => {
        not #view->exists
            ? fail(error_code_resNotFound, `File Not Found: ` + #view->path)
        
        local(prepend) = ''
        local(i) = #params->size
        while(#i--) => {
            local(item) = #params->get(loop_count)
            not #item->isA(::keyword)
                ? loop_continue
            
            #prepend->append(#item->name->asString + '=#' + loop_count + '->value,')
        }
        
        return(
            #prepend->size > 0
                ? sourcefile(`<?lasso local(` + #prepend + `) ?>` + #view->readString, #view->path, true, true)->invoke(:#params)
                | sourcefile(#view->readString, #view->path, true, true)->invoke
        )
    }
    
    public renderJSON(content::string) => {
        web_response->replaceHeader('Content-type'='application/json; charset=UTF-8')
        content_body = #content
    }
    public renderJSON(item) => .renderJSON(json_serialize(#item))
    
    public version => `0.9`
}
define rope => var(_rope_) || $_rope_ := rope_impl