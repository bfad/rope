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
        
        return (
            with key in #keys
            let validation = .ropeValidate(#key)
            where #validation != void 
            where (#validation->isNotA(::boolean) or #validation == false)
            select pair(#key->asString, #validation || `This field is invalid`)
        )->asStaticArray
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
define rv_notEmpty(value, message::string=`Field can not be empty`) => {
    (: ::void, ::null)->contains(#value->type) or #value->size == 0
        ? return #message
}

define rv_containedIn => type {
    data list, message::string

    public onCreate(list) => {
        .`list`    = #list
        .`message` = ``
        return self->\invoke
    }
    public onCreate(list, message::string) => {
        .`list`    = #list
        .`message` = #message

        return self->\invoke
    }
    public onCreate(value, list, message::string=``) => {
        return (.onCreate(#list, #message))->invoke(#value)
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


define rv_inRange => type {
    data
        series::generateSeries,
        message::string

    public onCreate(series::generateSeries, message::string=`Field must be between ` + math_min(#series->from, #series->to) + ` and ` + math_max(#series->from, #series->to)) => {
        .`series`  = #series
        .`message` = #message

        return self->\invoke
    }

    public onCreate(value, series::generateSeries, message::string=``) => {
        #message->size > 0
            ? return (.onCreate(#series, #message))->invoke(#value)
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