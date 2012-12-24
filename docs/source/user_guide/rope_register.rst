Registering Controllers and Routes
==================================
The most basic functionallity of any modern framework is matching URLs to the
code that needs to run to fulfill the request. Rope calls the code that runs
based on a URL request a "controller", and the mechanism it uses to match URLs
it calls "routes". The ``rope->register`` method is what is used to register a
block of code as a controller, and is used to register routes to a controller.


Registering a Controller
------------------------
The example below registers a block of code in the Rope application::

	rope->register(`home`) => {
		content_body = "Hello, World!"
	}

Your Rope application now has a controller named "home" that could be executed
by calling ``rope->controller('home')->invoke``. It is currently not attached to
any routes, and is therefore not able to be accessed by a URL. Let's fix that.


Registering a Route to a Controller
-----------------------------------
Once a controller has been registered into your application, it can then be
assigned to a route::

	rope->register(`home`, -routes=(:'/'))

The "routes" named parameter takes a staticarray of routes that Rope will use to
match URLs to that controller. (For details on the matching process, see the
discusison :ref:`below <ug-matching-url-to-routes>`.) It is important to note
that in this scenario, the controller must already be registered in your
application. However, registering the block of code and the routes can all be
done in one step.


.. _rope-register-block-and-routes:

Registering a Controller Code Block and Routes
----------------------------------------------
It is possible to register a controller with a block of code and the routes it
responds all at once::

	rope->register(`hello-name`, -routes(:'/:name')) => {
		content_body = "Hello, " + rope->param('name')
	}

In this example, I have registered a code block and its route all at once. This
route is a wild-card route that will match any URL that is only one level deep.
In other words, it would match "http://example.com/John" but not 
"http://example.com/John/Doe". Wild-card routes allow you to reference the part
of the path they represent by their name using the ``rope->param`` method.

.. note::
	Now that the controller has been registered, I could register additional routes
	later on if I wanted to using the sytax shown
	:ref:`above <rope-register-block-and-routes>`.


.. _ug-matching-url-to-routes:

How URLs Are Matched to Routes
------------------------------
Since routes are used to match the code that should be run when a URL is
requested, it is important to understand them and get them right. Rope will
throw an error if no route matches the URL, or if it finds more than one match.

In order for a route to match a URL, it must have the same number of parts as
the URL path. For example, the route "/rhino" will match URLs whose path are 
"/rhino" or "/rhino/" since each only have one part and that part matches the
route, but it wouldn't match "/rhino/horn" since it has more parts than the
route.

Routes can be used to match URL paths exactly, but they can also contain
wild-card parts that can match anything for that part of the URL path. These
wild-cards are specified by prefixing the name of that part with a colon. (These
names are matched as URL parameters later.) When matching routes with wild-card
parts, the matcher prefers those routes whose earliest parts match non-wildcard
parts. This means that for two routes "/prada/:id" and "/:designer/shoes", the
URL path "/prada/shoes" will match the first - "/prada/:id".