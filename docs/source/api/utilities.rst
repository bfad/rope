utilities
=========

.. method:: rope_cycle(...)

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
    
.. method:: rope_hash(plain::string, salt::string, cost::integer =?)

    A helper method that takes in plaintext, a salt, and number of iterations and
    encodes the plaintext with the salt in RIPEMD160 hash the specified number of
    iterations and then returns the whole thing in hexadecimal.
    
.. method:: rope_randomASCII(len::integer)

    Returns the specified number of random characters - each character is from the
    printable ASCII character set.
    
