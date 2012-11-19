define rope_impl => type {
    data private controllers::map = map,
         private routes     ::map = map
        
    public register(controller::string, -routes::staticarray=(:)) => {
        givenBlock? .controllers->insert(#controller=givenBlock)
        
        not givenBlock and #routes->size == 0
            ? fail(`This method requires you to either also pass routes or a givenBlock`)
    
        not .controllers->contains(#controller)
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
            
            #path = #path->split(`/`)
            not .'routes'->contains(#path->size)
                ? .'routes'->insert(#path->size=array)
            
            .'routes'->get(#path->size)->insert(#path=#controller)
        }
    }
    
    public matchRoute(path::string) => {
        local(path_split) = #path->asCopy->remove(1,1)&split('/')
        local(i)          = #path_split->size
        local(found)      = .routes->find(#i)
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
}
define rope => var(_rope_) || $_rope_ := rope_impl