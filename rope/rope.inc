define rope_thread => thread {
    data private controllers ::map = map,
         private routes      ::map = map,
         private code_paths  ::map = map,
         private configLoaded::boolean = false,
         private config      ::map = map(
                `app-name`         = pair(`UnNamed`,::string),
                `sessions-enabled` = pair(true, ::boolean),
                `sessions-expires` = pair(1440, ::integer)
            )

    public controllers  => .`controllers`
    public routes       => .`routes`
    public code_paths   => .'code_paths'
    public configLoaded => .`configLoaded`
    
    public controller(key::string)      => .`controllers`->get(#key)
    public controllerExist(key::string) => .`controllers`->contains(#key)
    public insertController(item::pair, path::string) => {
        .`controllers`->insert(#item)
        .`code_paths` ->insert(#item->first=#path)
    }
    public removeController(key::string) => {
        .`controllers`->remove(#key)
        .`code_paths` ->remove(#key)
    }
    public removeAllControllers => {
        .'controllers' = map
        .`code_paths`  = map
        .`routes`      = map
    }
    
    public codePath(key::string) => .`code_paths`->get(#key)
    
    public insertRoute(item::pair) => {
        local(path_size) = #item->first->size
        not .`routes`->contains(#path_size)
            ? .`routes`->insert(#path_size=array)

        not .`routes`->get(#path_size)->contains(#item)
            ? .`routes`->get(#path_size)->insert(#item)
    }
    public findRoutesSized(key::integer) => .`routes`->find(#key)
    
    public config(key::string) => .`config`->find(#key)->first
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
define rope_impl => type {
    data public urlParams::array = array
    
    public invoke => {
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
    
    public config(...) => rope_thread->config(:#rest)
    
    public register(controller::string, -routes::staticarray=(:)) => {
        givenBlock? rope_thread->insertController(#controller=givenBlock, .absolutePath(currentCapture->continuation->callsite_file))
        
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
    public reload(controller::string) => {
        local(path) = .controllerPath(#controller)
        sourcefile(file(#path)->readString, #path, false, false)->invoke
    }
    public reload => {
        local(paths)      = rope_thread->code_paths->values
        local(paths_done) = set
        with path in #paths where not #paths_done->contains(#path) do {
            #paths_done->insert(#path)
            sourcefile(file(#path)->readString, #path, false, false)->invoke
        }
    }

    public deregister(controller::string) => {
        rope_thread->removeController(#controller)
    }
    public deregisterAll => rope_thread->removeAllControllers
    
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
        #found->size == 0
            ? fail(error_code_fileNotFound, `No matching route found`)
        #found->size > 1
            ? fail(`Multiple routes matched the given path`)
            
        return #found->first
    }
    
    
    public controller     (key::string) => rope_thread->controller(#key)
    public controllerExist(key::string) => rope_thread->controllerExist(#key)
    public controllerPath (key::string) => rope_thread->codePath(#key)

    public routesSized(size::integer) => rope_thread->findRoutesSized(#size)
    
    
    public protocol => (web_request->isHttps ? 'https://' | 'http://')
    public method   => web_request->requestMethod
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
    
    public urlParam  (key::string, -selectAll::boolean=false) => .getParam('urlParams'  , #key, #selectAll)
    public queryParam(key::string, -selectAll::boolean=false) => .getParam('queryParams', #key, #selectAll)
    public postParam (key::string, -selectAll::boolean=false) => .getParam('postParams' , #key, #selectAll)
    public param     (key::string, -selectAll::boolean=false) => .getParam('params'     , #key, #selectAll)
    
    private getParam(type::string, key::string, selectAll::boolean) => {    
        if(#selectAll) => {
            return (
                with param in self->\(tag(#type))->invoke
                where #param->first == #key
                select #param->second
            )->asStaticArray
        }
        
        with param in self->\(tag(#type))->invoke
        where #param->first == #key
        do { return #param->second }
        
        return void
    }
    
    
    public continueSession => {
        not .config(`sessions-enabled`)? return
        
        session_start(`RopeSession`, -expires=.config(`sessions-expires`), -useCookie)
        if(session_result != "load") => {
            session_addVar(-name='ANDISessionID', '_sess_rope')
            session_addVar(-name='ANDISessionID', '_sess_user')
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
    public user=(rhs::trait_serializable) => {
        not .config(`sessions-enabled`)
            ? fail(`Sessions are disabled`)
        $_sess_user = #rhs
    }
    
    
    private viewForPath(path::string)::file => file(file_forceRoot('/../views/' + #path))
    
    public render(path::string, ...) => { content_body = .include(.viewForPath(#path), #rest) }
    public render(view::file  , ...) => { content_body = .include(#view, #rest) }
    
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
    
    public version => `0.7`
}
define rope => var(_rope_) || $_rope_ := rope_impl