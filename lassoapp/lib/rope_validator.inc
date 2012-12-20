// Needed for range validations
define generateSeries->from => .`from`
define generateSeries->to   => .`to`
define generateSeries->by   => .`by`


define rope_validator => type {
    data
        private validations::map,
        private values     ::map=map,
        private failed     ::staticarray

    // Getters
    public
        validations => .`validations`,
        values      => .`values`,
        failed      => .`failed`
    
    
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
    
    public allValid => {
        not .hasAllValidRun?
            .`failed` = (
                with key in .values->keys
                where not .validate(#key)
                select #key
            )->asStaticArray
        
        return (.failed->size == 0)
    }
    
    public hasAllValidRun => boolean(.`failed`->isA(::staticarray))
    
    public validate(key::string) => {
        local(
            validations = .validations->find(#key),
            value       = .values->find(#key),
        )
        
        // No validations registered for the key
        void == #validations
            ? return true
        
        local(
            val_type   = #validations->find(`type`),
            val_min    = #validations->find(`min`),
            val_max    = #validations->find(`max`),
            val_range  = #validations->find(`range`),
            val_custom = #validations->find(`custom`),
        )
        
        // Validate type
        if(#val_type->isA(::tag)) => {
            not #value->isA(#val_type)
                ? return false
        else(#val_type)
            not #val_type->contains(#value->type)
                ? return false
        }
        #value->isA(::null) or #value->isA(::void)
            ? return true
        
        
        // Validate min
        #val_min->isA(::integer) and (#value->isA(::trait_finite)? #value->size | #value) < #val_min
            ? return false
            
            
        // Validate max
        #val_max->isA(::integer) and (#value->isA(::trait_finite)? #value->size | #value) > #val_max
            ? return false
            
            
        // Validate range
        if(#val_range->isa(::generateSeries)) => {
            local(min) = math_min(#val_range->from, #val_range->to)
            local(max) = math_max(#val_range->from, #val_range->to)
            local(val) = (#value->isA(::trait_finite)? #value->size | #value)
            
            #val < #min or #val > #max
                ? return false
        }
        
        
        // Validate custom
        #val_custom->isA(::capture)
            ? return boolean(#val_custom(#value, .values))
        
        return true
    }
}