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