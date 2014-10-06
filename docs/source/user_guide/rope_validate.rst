Validations
===========
Rope provides a way to easily create custom validations on the data members of
your own custom types. Once your type imports and implements
``trait_rope_objectValidator`` you can easily ascertain whether an object is
setup correclty by calling the ``trait_rope_objectValidator->ropeIsValid``
method. This section of the documentation will walk you through how to implement
the trait and use some of the validation methods that come with the library.


Implementing trait_rope_objectValidator
---------------------------------------
To implement ``trait_rope_objectValidator``, you must have a member method that
returns a map. It expects that the keys to the map are either strings or tags
that correspond to data member getter methods you want to test. The values for
the keys in the map consist of the validator that will be run.

Validators can be either of type ``capture`` or ``memberstream`` and will be
passed the value produced by calling the method specified by the map key. If the
value passes the validation, ``void`` will be returned. If it fails, it returns
a string with a message of what failed.

Below is a small type that implements ``trait_rope_objectValidator`` with a
validator (``rv_notEmpty``) that tests that a course object has a name::

   define course => type {
       trait { import trait_rope_objectValidator }

       data public name

       public ropeValidations => map(::name = rv_notEmpty)
   }


Built-in Validators
-------------------
The rope validator library comes with a bunch of predefined validators that can
be used by invoking one of the methods detailed below. The documentation that
follows will detail configuration options that can be passed to the method to
setup the validator as well as what the returned validator checks. Note that
each method allows you to specify the message string to return upon failure by
passing in the string to the ``-message`` named parameter.

.. method:: rv_notEmpty(-message::string=`Field can not be empty`)

   Creates a validator that passes as long as the value is not ``void``,
   ``null``, or has a ``size`` member method return 0.


.. method:: rv_containedIn(list, -message::string=`Field must be one of ` + #list->join(', '))
.. method:: rv_notContainedIn(list, -message::string=`Field must not be one of ` + #list->join(', '))

   Requires a container of values passed as the first argument. Depending on the
   method, creates a validator that passes if the value is or is not contained
   in the values passed in to the "list" parameter.


.. method:: rv_inRange(series::generateSeries, -message::string=`Field must be between ` + #series->from + ` and ` + #series->to)
.. method:: rv_notInRange(series::generateSeries, -message::string=`Field must not be between ` + #series->from + ` and ` + #series->to)

   Requires a range be passed as the first argument. Depending on the method, it
   returns a validator that passes if the value is inside or is outside of the
   given range.


.. method:: rv_greaterThan(bound::integer, -message::string=`Field must be greater than ` + #bound)
.. method:: rv_greaterThan(bound::decimal, -message::string=`Field must be greater than ` + #bound)
.. method:: rv_lessThan(bound::integer, -message::string=`Field must be less than ` + #bound)
.. method:: rv_lessThan(bound::decimal, -message::string=`Field must be less than ` + #bound)
   
   Requires either an integer or decimal value to be compared to the value being
   validated. Depending on the method, it returns a validator that passes if the
   value is greater than or less than the value passed to "bound".


.. method:: rv_isValue(expected, -message::string=`Field must be ` + #expected)
.. method:: rv_isNotValue(expected, -message::string=`Field must not be ` + #expected)

   Requires a value to compare for equality (``==`` or ``!=``) to the value
   being validated. Depending on the method, it returns a validator that passes
   if the value is equal to or not equal to the value passed to "expected".


.. method:: rv_isExactly(expected, -message::string=`Field must be ` + #expected)
.. method:: rv_isNotExactly(expected, -message::string=`Field must not be ` + #expected)

   Requires a value to compare for exact equality (``===`` or ``!==``) to the
   value being validated. Depending on the method, it returns a validator that
   passes if the value is exactly equal to or not exactly equal to the value
   passed to "expected".


.. method:: rv_isA(expected::tag, message::string=`Field must be a ` + #expected)
.. method:: rv_isNotA(expected::tag, message::string=`Field must not be a ` + #expected)

   Requires a tag to be used for type comparison. Depending on the method, it
   returns a validator that passes if the value is or is not of the specified
   type.


Custom Validators
-----------------
You can create your own custom validators by specifying a ``capture`` as the
value in the "ropeValidators" map. (You can also create your own methods that
return either a ``capture`` or ``memberstream`` and invoke those methods to get
the custom validator.) The example below tests that the value stored in
"squaredNumber" is in fact the square of an integer. The value being validated
will be passed to the capture as the first parameter, so ``#1`` must be used to
access it.

::

   define myType => type {
       trait { import trait_rope_objectValidator }

       data public squaredNumber

       public ropeValidations => map(
           ::name = {
               math_sqrt(#1) != integer(math_sqrt(#1))
                  ? return `Should be the value of a whole number squared`
           }
       )
   }


Using Built-in Validators Inside Custom Validators
--------------------------------------------------
Each of the built-in validators has a form that can be used inside a custom
validator. Just call the validator method with the value you want to check as
the first argument. Instead of returning the validation it will call it on the
value passed in. In the example below, the "myDate" type has a custom validator
for the "day" data member that checks that it is in the correct range given the
month.

::

   define myDate => type {
       trait { import trait_rope_objectValidator }

       data
           public month
           public day

       public ropeValidations => map(
           ::month = rv_inRange(1 to 12),
           ::day   = {
               local(value) = #1
               match(.month) => {
               case(2)
                   return rv_inRance(#value, 1 to 29)

               case(1, 3, 5, 7, 8, 10, 12)
                   return rv_inRance(#value, 1 to 31)

               case
                   return rv_inRance(#value, 1 to 30)
               }
           }
       )
   }

