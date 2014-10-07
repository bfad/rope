rope_user
=========

.. class:: trait_rope_user

    The ``rope->user=`` method requires that the object it is being set to implement
    this trait. This allows for custom user types to be used, but also require those
    types to have certain methods. (In this case, it's important for them to have
    methods to implement ``trait_serializable`` and an ``isAuthenticated`` method.)
    
    Imported Traits:

        * trait_serializable

    Required Methods:

        * isAuthenticated()

.. class:: rope_user

    A very basic user type that implments ``trait_rope_user`` and tracks ID, first
    name, lastname, and username. While it can be used if this is all you need, it's
    meant to be a reference for web applications to create their own user types.
    
    .. method:: acceptDeserializedElement(d::serialization_element)

    .. method:: id()

    .. method:: id=(id::integer)

    .. method:: isAuthenticated()

    .. method:: nameFirst()

    .. method:: nameFirst=(nameFirst::string)

    .. method:: nameLast()

    .. method:: nameLast=(nameLast::string)

    .. method:: oncreate()

    .. method:: oncreate(id::integer)

    .. method:: oncreate(id::integer, name_first::string, name_last::string)

    .. method:: oncreate(id::integer, name_first::string, name_last::string, username::string)

    .. method:: oncreate(id::integer, username::string)

    .. method:: serializationElements()

    .. method:: username()

    .. method:: username=(username::string)

