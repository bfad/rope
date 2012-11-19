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
            
            .'routes'->get(#path->size)->insert(#path)
        }
    }
}
define rope => var(_rope_) || $_rope_ := rope_impl