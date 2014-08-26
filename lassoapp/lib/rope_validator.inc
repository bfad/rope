define trait_rope_objectValidator => trait {

    require ropeValidations()::map
    
    provide 
        ropeValidate(item::string) => .ropeValidate(tag(#item)),
        ropeValidate(item::tag)    => {
            local(code) = .ropeValidations->find(#item)
            match(#code->type) => {
            case(::capture)
                return #code->detach->invoke(self->\(#item)->invoke)
            case(::memberstream)
                return #code->invoke(self->\(#item)->invoke)
            case
                return #code
            }
        }

    provide ropeValidate(keys::trait_ForEach=.ropeValidations->eachKey) => {
        #keys->isA(::trait_finiteForEach) and #keys->size == 0
            ? fail(`Nothing to validate`)
        
        local(ret) = map
        
        with key in #keys
        let validation = .ropeValidate(#key)
        where #validation != void 
        where (#validation->isNotA(::boolean) or #validation == false)
        do #ret->insert(#key->asString = #validation || `This field is invalid`)
        
        return #ret
    }

    provide ropeIsValid() => .ropeValidate->size == 0

    provide
        ropeIsValid(item::string) => .ropeIsValid(tag(#item)),
        ropeIsValid(item::tag)    => {
            local(result) = .ropeValidate(#item)
            #result == void or #result === true
                ? return true

            return false
        }
}

// Needed for range validations
define generateSeries->from => .`from`
define generateSeries->to   => .`to`
define generateSeries->by   => .`by`


/*
* Validators
*/
define rv_notEmpty => \rv_notEmpty
define rv_notEmpty(value, -message::string=`Field can not be empty`) => {
    (: ::void, ::null)->contains(#value->type) or #value->size == 0
        ? return #message
}

define rv_containedIn => type {
    data list, message::string

    public onCreate(list, -message::string=``) => {
        .`list`    = #list
        .`message` = #message

        return self->\invoke
    }
    public onCreate(value, list, -message::string=``) => {
        return (.onCreate(#list, -message=#message))->invoke(#value)
    }

    public invoke(value, message::string=``) => {
        .`list` >> #value
            ? return void
        
        #message->size > 0
            ? return #message
        .`message`->size > 0
            ? return .`message`
        
        return `Field must be one of ` + .`list`->join(', ')
    }
}

define rv_notContainedIn => type {
    data list, message::string

    public onCreate(list, -message::string=``) => {
        .`list`    = #list
        .`message` = #message

        return self->\invoke
    }
    public onCreate(value, list, -message::string=``) => {
        return (.onCreate(#list, -message=#message))->invoke(#value)
    }

    public invoke(value, message::string=``) => {
        .`list` !>> #value
            ? return void
        
        #message->size > 0
            ? return #message
        .`message`->size > 0
            ? return .`message`
        
        return `Field must not be one of ` + .`list`->join(', ')
    }
}

define rv_inRange => type {
    data
        series::generateSeries,
        message::string

    public onCreate(series::generateSeries, -message::string=`Field must be between ` + math_min(#series->from, #series->to) + ` and ` + math_max(#series->from, #series->to)) => {
        .`series`  = #series
        .`message` = #message

        return self->\invoke
    }

    public onCreate(value, series::generateSeries, -message::string=``) => {
        #message->size > 0
            ? return (.onCreate(#series, -message=#message))->invoke(#value)
            | return (.onCreate(#series))->invoke(#value)
    }

    public invoke(value, message::string=``) => {
        local(min) = math_min(.`series`->from, .`series`->to)
        local(max) = math_max(.`series`->from, .`series`->to)
        local(val) = #value
            
        #val < #min or #val > #max
            ? return #message || .`message`
    }
}

define rv_notInRange => type {
    data
        series::generateSeries,
        message::string

    public onCreate(series::generateSeries, -message::string=`Field must not be between ` + math_min(#series->from, #series->to) + ` and ` + math_max(#series->from, #series->to)) => {
        .`series`  = #series
        .`message` = #message

        return self->\invoke
    }

    public onCreate(value, series::generateSeries, -message::string=``) => {
        #message->size > 0
            ? return (.onCreate(#series, -message=#message))->invoke(#value)
            | return (.onCreate(#series))->invoke(#value)
    }

    public invoke(value, message::string=``) => {
        local(min) = math_min(.`series`->from, .`series`->to)
        local(max) = math_max(.`series`->from, .`series`->to)
        local(val) = #value
            
        #val < #min or #val > #max
            ? return

        return #message || .`message`
    }
}

define rv_greaterThan => type {
    data
        bound,
        message::string

    public onCreate(bound::integer, -message::string=`Field must be greater than ` + #bound) => {
        .`bound`  = #bound
        .`message` = #message

        return self->\invoke
    }

    public onCreate(bound::decimal, -message::string=`Field must be greater than ` + #bound) => {
        .`bound`  = #bound
        .`message` = #message

        return self->\invoke
    }

    public onCreate(value, bound::integer, -message::string=``) => {
        #message->size > 0
            ? return (.onCreate(#bound, -message=#message))->invoke(#value)
            | return (.onCreate(#bound))->invoke(#value)
    }

    public onCreate(value, bound::decimal, -message::string=``) => {
        #message->size > 0
            ? return (.onCreate(#bound, -message=#message))->invoke(#value)
            | return (.onCreate(#bound))->invoke(#value)
    }

    public invoke(value, message::string=``) => {
        #value <= .`bound`
            ? return #message || .`message`
    }
}

define rv_lessThan => type {
    data
        bound,
        message::string

    public onCreate(bound::integer, -message::string=`Field must be less than ` + #bound) => {
        .`bound`  = #bound
        .`message` = #message

        return self->\invoke
    }

    public onCreate(bound::decimal, -message::string=`Field must be less than ` + #bound) => {
        .`bound`  = #bound
        .`message` = #message

        return self->\invoke
    }

    public onCreate(value, bound::integer, -message::string=``) => {
        #message->size > 0
            ? return (.onCreate(#bound, -message=#message))->invoke(#value)
            | return (.onCreate(#bound))->invoke(#value)
    }

    public onCreate(value, bound::decimal, -message::string=``) => {
        #message->size > 0
            ? return (.onCreate(#bound, -message=#message))->invoke(#value)
            | return (.onCreate(#bound))->invoke(#value)
    }

    public invoke(value, message::string=``) => {
        #value >= .`bound`
            ? return #message || .`message`
    }
}

define rv_isValue => type {
    data
        expected,
        message::string

    public onCreate(expected, -message::string=`Field must be ` + #expected) => {
        .`expected` = #expected
        .`message`  = #message

        return self->\invoke
    }
    public onCreate(value, expected, -message::string=`Field must be ` + #expected) => {
        return (.onCreate(#expected, -message=#message))->invoke(#value)
    }

    public invoke(value, -message::string=``) => {
        #value == .`expected` ? return

        return #message || .`message`
    }
}

define rv_isNotValue => type {
    data
        expected,
        message::string

    public onCreate(expected, -message::string=`Field must not be ` + #expected) => {
        .`expected` = #expected
        .`message`  = #message

        return self->\invoke
    }
    public onCreate(value, expected, -message::string=`Field must not be ` + #expected) => {
        return (.onCreate(#expected, -message=#message))->invoke(#value)
    }

    public invoke(value, -message::string=``) => {
        #value != .`expected` ? return

        return #message || .`message`
    }
}

define rv_isExactly => type { parent rv_isValue
    public onCreate(...) => ..onCreate(:#rest || (:))

    public invoke(value, -message::string=``) => {
        #value === .`expected` ? return

        return #message || .`message`
    }
}

define rv_isNotExactly => type { parent rv_isNotValue
    public onCreate(...) => ..onCreate(:#rest || (:))

    public invoke(value, -message::string=``) => {
        #value !== .`expected` ? return

        return #message || .`message`
    }
}

define rv_isA => type {
    data
        expected::tag,
        message::string

    public onCreate(expected::tag, -message::string=`Field must be a ` + #expected) => {
        .`expected` = #expected
        .`message`  = #message

        return self->\invoke
    }
    public onCreate(value, expected::tag, -message::string=`Field must be a ` + #expected) => {
        return (.onCreate(#expected, -message=#message))->invoke(#value)
    }

    public invoke(value, message::string=``) => {
        #value->isA(.`expected`) ? return

        return #message || .`message`
    }
}

define rv_isNotA => type {
    data
        expected::tag,
        message::string

    public onCreate(expected::tag, -message::string=`Field must not be a ` + #expected) => {
        .`expected` = #expected
        .`message`  = #message

        return self->\invoke
    }
    public onCreate(value, expected::tag, -message::string=`Field must not be a ` + #expected) => {
        return (.onCreate(#expected, -message=#message))->invoke(#value)
    }

    public invoke(value, message::string=``) => {
        #value->isNotA(.`expected`) ? return

        return #message || .`message`
    }
}