define rope_thread => thread {
    data private controllers::map = map,
         private routes     ::map = map

    public controllers => .`controllers`
    public routes      => .`routes`
    
    public controller(key::string)      => .`controllers`->get(#key)
    public insertController(item::pair) => .`controllers`->insert(#item)
    public controllerExist(key::string) => .`controllers`->contains(#key)
    
    public insertRoute(item::pair) => {
        not .`routes`->contains(#item->first->size)
            ? .`routes`->insert(#item->first->size=array)

        .`routes`->get(#item->first->size)->insert(#item)
    }
    public findRoutesSized(key::integer) => .`routes`->find(#key)
}
define rope_impl => type {
    data public urlParams::array = array
    
    public invoke => {
        local(route) = .matchRoute(.path)
        
        //Setup URL parameters
        local(elements) = .path->asCopy->remove(1,1)&split('/')
        local(i) = #elements->size
        while(#i--) => {
            local(part) = #route->first->get(loop_count)->asCopy
            if(`:` == #part->get(1)) => {
                #part->remove(1,1)
                .'urlParams'->insert(#part=#elements->get(loop_count))
            }
        }
        return .controller(#route->second)->invoke
    }
    
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
            #path->get(#path->size) == `/`
                ? #path->remove(#path->size, 1)
            
            rope_thread->insertRoute(#path->split(`/`)=#controller)
        }
    }
    
    public matchRoute(path::string) => {
        local(path_split) = #path->asCopy->remove(1,1)&split('/')
        local(i)          = #path_split->size
        local(found)      = rope_thread->findRoutesSized(#i)
        local(good, better, ii)
        
        while(#i-- and #ii := #found->size) => {
            local(pi) = loop_count
            #good     = array
            #better   = array
            
            while(#ii--) => {
                #found->get(loop_count)->first->get(#pi) == #path_split->get(#pi)
                    ? #better->insert(#found->get(loop_count))
                #found->get(loop_count)->first->get(#pi)->first == ':'
                    ? #good->insert(#found->get(loop_count))
            }
            #better->size
                ? #found = #better
                | #found = #good
        }
        #found->size == 0
            ? fail(error_code_invalidParameter, `No matching route found`)
        #found->size > 1
            ? fail(`Multiple routes matched the given path`)
            
        return #found->first
    }
    
    
    public controller     (key::string) => rope_thread->controller(#key)
    public controllerExist(key::string) => rope_thread->controllerExist(#key)

    public routesSized(size::integer) => rope_thread->findRoutesSized(#size)
    
    
    public protocol => (web_request->isHttps ? 'https://' | 'http://')
    public method   => web_request->requestMethod
    public hostname => web_request->httpHost
    public path     => {
        local(path) = web_request->requestUri->split('?')->get(1)->split('/')
        #path->last->contains('.')
            ? #path->last = #path->last->split('.')->remove&join('.')
        
        #path->size > 2 and #path->last->isEmpty ? #path->remove
        return #path->join('/')
    }
    public extension => {
        local(ext) = web_request->requestUri->split('?')->get(1)->split('/')->last->split('.')
        #ext->size > 1
            ? return #ext->last
        return null
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
    
    public render(path::string) => .render(file(file_forceRoot('/../views/' + #path)))
    public render(view::file) => {
        not #view->exists
            ? fail(error_code_resNotFound, `File Not Found: ` + #view->path)
        
        content_body = sourcefile(#view->readString, #view->path, true, true)->invoke
    }
    
    public version => `0.1`
}
define rope => var(_rope_) || $_rope_ := rope_impl