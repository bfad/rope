define rope => type {
    public route(...) => {
        #rest->size == 0
            ? fail(error_code_methodNotFound, `Definition Not Found: rope->route() Candidates were: rope->route(route::string, ...)`)
        fail_if(not givenBlock, `The route member method requires a givenBlock`)

        // Not sure why, but while beats the socks off loop and query expressions in performance
        local(i) = #rest->size
        while(#i--) => {
            not #rest->get(loop_count)->isA(::string)
                ? fail(error_code_invalidParameter, `Invalid parameter: The route member method was passed an object that was not a string`)
        }
    }
}