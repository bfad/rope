Inpsecting an HTTP Request
==========================
Rope has many methods to inspect various parts of an HTTP request. Many of these
methods are similar to member methods of the ``web_request`` type, but with
additional functionality.


Inspecting the URL Request
--------------------------
Each part of the URL can be inspected by one of the following methods:


.. method:: rope->protocol

   This method returns the protocol part of the URL request: "https://" if the
   request is over an SSL connection and "http://" if it is not.

.. method:: rope->method

   This method returns the HTTP method used for the HTTP request (eg "GET" or
   "POST").

.. method:: rope->hostname

   This method returns the hostname portion of the URL request (eg 
   "www.example.com").

.. method:: rope->path

   This method returns the path portion of the URL request. For example, if the
   URL is "http://www.example.com/my/path.json?foo=bar", this method would
   return "/my/path.json".

.. method:: rope->extension

   This method returns the file extension of the URL path or "html" if the path
   doesn't have an extension. For example, it returns "json" if the URL is
   "http://www.example.com/my/path.json?foo=bar".


Inspecting the Request Parameters
---------------------------------
Rope has convenience methods to inspect HTTP query and post parameters. It has
similar methods for inspecting :ref:`Rope URL parameters
<rope-register-block-and-routes>`.


.. method:: rope->urlParams
   
   This method returns an array of url paramaters.

.. method:: rope->queryParams
   
   This method returns an array of query paramaters.

.. method:: rope->postParams
   
   This method returns an array of post paramaters.

.. method:: rope->params
   
   This method returns an array of all the paramaters.


Rope also has methods for extracting specific values using the parameter name.
Once again, there are different methods for each parameter type as well as a
method that will search all parameter types. Each of them take the same options:

.. method:: rope->urlParam(key::string, -as::tag=::stringOrNull, -default::any=void, -selectAll::boolean=false)
.. method:: rope->queryParam(key::string, -as::tag=::stringOrNull, -default::any=void, -selectAll::boolean=false)
.. method:: rope->postParam(key::string, -as::tag=::stringOrNull, -default::any=void, -selectAll::boolean=false)
.. method:: rope->param(key::string, -as::tag=::stringOrNull, -default::any=void, -selectAll::boolean=false)

   Each method requires the name of the parameter whose value(s) you want to
   extract. The "-as" parameter allows for converting the value to a type by
   passing a tag of the type name or a tag of a method name. The value returned
   to you will be the result of passing the parameter value to either the type's
   creator method or the specified method. The default is to return the value as
   a string or to return ``null`` if the parameter is passed without a value. If
   the parameter has not been passed, then these methods will return the value
   you set in the "-default" parameter (which defaults to ``void``). If multiple
   parameters have the same name, only the value from the first parameter found
   will be returned unless the "-selectAll" flag is passed to the method in
   which case a ``staticarray`` of all the values will be returned.


File Uploads
------------
Rope also has a convience method for inspecting any files uploaded by the web
request.

.. method:: rope->fileUploads

   This method returns the same information as ``web_request->fileUploads`` but
   as an array of maps rather than an array of arrays with pairs. (See the
   documentation in `the LassoGuide
   <http://lassoguide.com/operations/requests-responses.html#uploading-files>`_
   for map keys and their corresponding values.)
