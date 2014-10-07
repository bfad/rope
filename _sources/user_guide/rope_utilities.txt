Utility Methods
===============
Rope comes with some utility methods that you may find handy. These methods are
documented below with both a description of their usage and an example.

.. method:: rope_randomASCII(len::integer)
   :noindex:

   Returns the specified number of random characters - each character is from
   the printable ASCII character set. Please note that your ouput will not match
   the output in the example below as the point of this method is to output
   random characters.

   Example::

      rope_randomASCII(8)
      // => j^e2Z1w~
   

.. method:: rope_randomAlphaNumeric(len::integer)
   :noindex:

   Returns the specified number of random characters - each being one of 0-9,
   a-z, or A-Z. Please note that your ouput will not match
   the output in the example below as the point of this method is to output
   random characters.

   Example::

      rope_randomAlphaNumeric(8)
      // => apS292Ow


.. method:: rope_hash(plain::string, salt::string, cost::integer =?)
   :noindex:

   A helper method that takes in a plaintext string, a salt string, and the
   number of iterations. It then encrypts the plaintext using the salt with the
   RIPEMD160 hashing algorithm the specified number of iterations. It returns
   the result in hexadecimal.

   Example::

      rope_hash('My Secret', 'Mmmm...Tasty', 10)
      // => 07679C7DB191A15386CB4317D9EC00C074AA4C36


.. method:: rope_cycle(...)
   :noindex:

   A helper method that allows for cycling through a list of objects. The
   ``rope_cycle`` method takes in an indeterminate number of parameters and
   returns an anonymous capture. When this capture is invoked it returns the
   next item in the list of objects. Once it reaches the end of the list, it
   just starts back at the beginning.
   
   Example::
   
      local(colors) = rope_cycle('red', 'green', 'blue')
      #colors->invoke
         // => red
      #colors()
         // => green
      #colors->invoke
         // => blue
      #colors()
         // => red


   
.. method:: dir_import(d::dir, ext::staticarray =?)

   This method takes a dir object. Starting with that directory, it calls
   ``file_import`` on all files whose extensions match those passed in the "ext"
   parameter's staticarray, and then proceeds to do the same thing recursively
   through all the other directories contained in the directory.
   
   The default value of "ext" is ``(:'lasso', 'inc')``.

   Example::

      dir_import(dir('//my/cool/path/'))
   

.. method:: file_import(f::file)

   This method takes in an file object and executes the contents of the file as
   Lasso source code. (It's similar to the ``library`` method.)

   Example::

      file_import(dir('//my/cool/file.lasso'))


.. method:: stringOrNull(value)
.. method:: stringOrNull(value::string)

   This method takes in an object and returns null if it is empty or the value
   as a string if it is not. It is meant to be used in the ``-as`` paramater of
   a ``rope->param()`` (and it's siblings) method call. Ex::
      
      rope->param('rhino', -as=::stringOrNull)

   But it can also be used by itself::

      stringOrNull(#my_value)
   

.. method:: integerOrNull(value)
.. method:: integerOrNull(value::integer)

   This method takes in an object and returns null if it is empty or the value
   as an integer if it is not. It is meant to be used in the ``-as`` paramater
   of a ``rope->param()`` (and it's siblings) method call. Ex::
      
      rope->param('rhino', -as=::integerOrNull)

   But it can also be used by itself::

      integerOrNull(#my_value)


.. method:: decimalOrNull(value)
.. method:: decimalOrNull(value::decimal)

   This method takes in an object and returns null if it is empty or the value
   as a decimal if it is not. It is meant to be used in the ``-as`` paramater of
   a ``rope->param()`` (and it's siblings) method call. Ex::
      
      rope->param('rhino', -as=::integerOrNull)

   But it can also be used by itself::

      decimalOrNull(#my_value)


.. method:: dateOrNull(value)
.. method:: dateOrNull(value::date)
.. method:: dateOrNull(value::bytes)

   This method takes in an object and returns null if it is empty or the value
   as a date if it is not. It is meant to be used in the ``-as`` paramater of a
   ``rope->param()`` (and it's siblings) method call. Ex::
      
      rope->param('rhino', -as=::dateOrNull)

   But it can also be used by itself::

      dateOrNull(#my_value)


.. method:: orNullWrap(value::void, method::tag)
.. method:: orNullWrap(value::null, method::tag)
.. method:: orNullWrap(value::string, method::tag)
.. method:: orNullWrap(value::bytes, method::tag)

   This method takes in an object of the specified type and returns null if it
   is empty or the value as returned by the invocation of the tag passed as the
   second parameter. (Note, this may actually be the "onCreate" method of a
   type.)
   
   It is meant to be used as the "private" method that gets called behind the
   scenes for the "...OrNull" methods: ``stringOrNull``, ``dateOrNull``, etc.