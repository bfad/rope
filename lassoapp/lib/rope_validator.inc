define rope_validator => type {
    data
        private validations::map,
        private values     ::map=map

    // Getters
    public
        validations => .`validations`,
        values      => .`values`
    
    
    public onCreate(validations::map, ...) => {
        #rest->size == 0
            ? fail(error_code_methodNotFound, error_msg_methodNotFound + `: Candidtates are rope_validator(validations::map, ...)`)
        
        local(i) = #rest->size
        while(#i--) => {
            local(item) = #rest->get(loop_count)
            not #item->isA(::keyword) and not #item->isA(::pair)
                ? fail(error_code_invalidParameter, error_msg_invalidParameter)
            
            .`values`->insert(#item->name->asString = #item->value)
        }
        .`validations` = #validations
    }
}