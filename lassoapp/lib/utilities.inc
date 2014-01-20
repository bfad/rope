define  string->encodeUrl(strict::boolean=true) => bytes(self)->encodeUrl(#strict)
define integer->encodeUrl(strict::boolean=true) => bytes(self->asString)

/**!
    A helper method that allows for cycling through a list of objects. The 
    ``rope_cycle`` method takes in an indeterminate number of parameters and returns
    an anonymous capture which returns the next item in the list of objects each
    time it is invoked. Once it reaches the end of the list, it just starts back at
    the beginning.

    Example::

        local(colors) = rope_cycle('red', 'green', 'blue')
        #colors->invoke
            // produces red
        #colors()
            // produces green
        #colors->invoke
            // produces blue
        #colors()
            // produces red
*/
define rope_cycle(...) => { 
    return { 
        local(i) = #rest->size
        while(#i--) => { yield #rest->get(loop_count) } 
        currentCapture->restart
    }->detach 
}

/**!
    A helper method that takes in plaintext, a salt, and number of iterations and
    encodes the plaintext with the salt in RIPEMD160 hash the specified number of
    iterations and then returns the whole thing in hexadecimal.
*/
define rope_hash(plain::string, salt::string, cost::integer=1) => {
    local(cyphertext) = #plain
    loop(#cost) => { #cyphertext = cipher_digest(#salt + #cyphertext, -digest='RIPEMD160') }
    return #cyphertext->encodeHex
}

/**!
    Returns the specified number of random characters - each character is from the
    printable ASCII character set.
*/
define rope_randomASCII(len::integer) => {
    local(ret) = ''
    while(#len--) => {
        #ret->append(bytes->import8bits(math_random(32,126))&asString)
    }
    return #ret
}

/**!
    Returns the specified number of random characters - each being one of 0-9, a-z,
    or A-Z
*/
define rope_randomAlphaNumeric(len::integer) => {
    local(ret) = ''
    while(#len--) => {
        match(math_random(1,99) % 3) => {
        case(0)
            #ret->append(bytes->import8bits(math_random(48, 57))&asString)
        case(1)
            #ret->append(bytes->import8bits(math_random(65, 90))&asString)
        case
            #ret->append(bytes->import8bits(math_random(97, 122))&asString)
        }
    }
    return #ret
}



define dir_import(d::dir, ext::staticarray=(:'lasso', 'inc')) => {
    with f in #d->eachFile
    where #ext->contains(#f->path->split('.')->last) 
    do file_import(#f)

    with f in #d->eachDir do dir_import(#f)
}
define file_import(f::file) => {
    sourcefile(#f->readString, #f->path, false, false)->invoke
}


/**!
This method takes in a bytes object and returns null if it is empty or the value
as returned by the invokation of the tag passed as the second parameter. (Note,
this may actually be the "onCreate" method of a type and not actual invoke-ing.)

It is meant to be used as the "private" method that gets called behind the
scenes for the "...OrNull" methods: ``stringOrNull``, ``dateOrNull``, etc.
*/
define orNullWrap(value::bytes , method::tag) => (#value->size == 0 ? null | \#method(#value))
define orNullWrap(value::string, method::tag) => (#value->size == 0 ? null | \#method(#value))
/**!
This method takes in a bytes object and returns null if it is empty or the value
as a string if it is not. It is meant to be used in the ``-as`` paramater of a
``rope->param()`` (and it's siblings) method call. Ex::
    
    rope->param('rhino', -as=::stringOrNull)
*/
define stringOrNull(value)         => orNullWrap(#value, ::string)
define stringOrNull(value::null)   => null
define stringOrNull(value::void)   => null
define stringOrNull(value::string) => #value
/**!
This method takes in a bytes object and returns null if it is empty or the value
as an integer if it is not. It is meant to be used in the ``-as`` paramater of a
``rope->param()`` (and it's siblings) method call. Ex::
    
    rope->param('rhino', -as=::integerOrNull)
*/
define integerOrNull(value)          => orNullWrap(#value, ::integer)
define integerOrNull(value::null)    => null
define integerOrNull(value::void)    => null
define integerOrNull(value::integer) => #value
/**!
This method takes in a bytes object and returns null if it is empty or the value
as an integer if it is not. It is meant to be used in the ``-as`` paramater of a
``rope->param()`` (and it's siblings) method call. Ex::
    
    rope->param('rhino', -as=::integerOrNull)
*/
define decimalOrNull(value)          => orNullWrap(#value, ::decimal)
define decimalOrNull(value::null)    => null
define decimalOrNull(value::void)    => null
define decimalOrNull(value::decimal) => #value
/**!
This method takes in a bytes object and returns null if it is empty or the value
as a date if it is not. It is meant to be used in the ``-as`` paramater of a
``rope->param()`` (and it's siblings) method call. Ex::
    
    rope->param('rhino', -as=::dateOrNull)
*/
define dateOrNull(value::bytes)  => dateOrNull(string(#value))
define dateOrNull(value::null)   => null
define dateOrNull(value::void)   => null
define dateOrNull(value::date)   => #value
define dateOrNull(value)         => orNullWrap(#value, ::date)