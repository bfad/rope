rope
====

.. class:: rope_thread

    A thread object that tracks configuration settings, routes, and controllers. 
    When not in produciton mode it also tracks files in LassoStartup. Due to the
    nature of thread objects, it can't reload changed files, but relies on the
    main thread to do the actual reloading.
    
    This thread object is what Rope uses internally, and is not meant to be 
    accessed by a user directly. All interaction with the framework is meant to go
    through the ``rope`` method.
    
    .. method:: config(...)

        This method expects an argument list of pairs whose first value is the name of
        the configuration being set and the second is the value to set it to. The list
        of configurations and their defaults is stored in a private ``config`` type
        variable. Errors will be thrown if the configuration doesn't exist or if the
        value passed for the configuration is not of the correct type.
        
    .. method:: config(key::string)

    .. method:: config=(config::map)

    .. method:: configLoaded()

    .. method:: configLoaded=(configLoaded::boolean)

    .. method:: controller(key::string)

    .. method:: controllerExist(key::string)

    .. method:: controllers()

    .. method:: controllers=(controllers::map)

    .. method:: findFilesToReload()

        Due to the nature of thread objects and that the files that need reloading
        could call methods that try to access ``rope_thread``, the thread object can't
        reload changed files, but it can return an array of files for the main thread
        to reload.
        
        While looking for files to reload, this method also sees if any files have
        been deleted and removes them from being tracked by Rope.
        
    .. method:: findRoutesSized(key::integer)

    .. method:: insertController(item::pair)

    .. method:: insertRoute(item::pair)

    .. method:: oncreate()

        When not in a poduction environment, Rope grabs the current modification times
        for all files in LassoStartup so that each time ``rope->invoke`` is called
        (every request), it will reload any files whose modification time has changed.
        
    .. method:: removeAllControllers()

    .. method:: removeController(key::string)

    .. method:: routeLink(key::string)

    .. method:: routeLinks()

    .. method:: routeLinks=(routeLinks::map)

    .. method:: routes()

    .. method:: routes=(routes::map)

    .. method:: startupFiles()

    .. method:: startupFiles=(startupFiles::map)

.. class:: rope_impl

    .. method:: absolutePath(path::string)

    .. method:: config(...)

    .. method:: configLoaded()

    .. method:: continueSession()

    .. method:: controller(key::string)

    .. method:: controllerExist(key::string)

    .. method:: deregister(controller::string)

    .. method:: deregisterAll()

    .. method:: env()

        This method returns "dev" if the ROPE_ENV environment variable has not been set.
        Otherwise it returns the value stored in the environment variable, even if that
        value is an empty string
        
    .. method:: extension()

    .. method:: fileUploads()

    .. method:: getParam(type::string, key::string, as::tag, default, selectall::boolean)

    .. method:: hostname()

    .. method:: include(path::string, ...)

        The ``include`` member method accepts either a file or a string which is a path
        to a file in the view folder. The string can be a relative path if this method
        is being called in a view itself and the file will be assumed to be relative to
        the current file. If passed an absolute path - a path starting with a single 
        slash (/) - it will be assumed that the path is actually relative from the view
        folder itself. A fully-qualified absolute path - a path that starts with two or
        more slashes - will be treated like an absolute path from the root of the file
        system.
        
        Optionally, this method can take any number of keyword parameters. These
        parameters are setup as local variables for the file they are including. The
        name of the local variable is the parameter name and the value for the variable
        is the corresponding parameter value. Essentially, this allows you to easily
        "pass" local variables for use inside your views.
        
    .. method:: include(view::file, ...)

    .. method:: include(view::file, params::staticarray)

    .. method:: invoke()

        This is where the magic all happens. Every web request that rope handles runs
        this method and follows these steps:
        
            #. If we are not running in production mode, it checks for any new files in
               LassoStartup or any modified existing files and loads them. 
        
            #. It matches the URL path to a route. If there are any URL parameters in 
               the path then it also loads them up.
        
            #. It continues the Rope Session as long as sessions aren't turned off.
        
            #. It executes the controller block that matches the route that was found.
        
    .. method:: link(key::string, index::integer =?, values::trait_positionallyKeyed =?)

    .. method:: link(key::string, index::integer)

    .. method:: link(key::string, values::trait_positionallyKeyed)

    .. method:: links()

    .. method:: links=(links::map)

    .. method:: matchRoute(path::array)

    .. method:: matchRoute(path::string)

        This method can take either a string that's the URL path or an array, each
        element of which is part of the path. The algorithm for matching routes is a bit
        complex:
        
            #. The Route must be the same number of parts as the path.
        
            #. Routes can have wild-card parts, indicated by the part starting with a
               colon. (These are matched as URL parameters later.) However, the matcher
               prefers those routes whose earliest parts match non-wildcard parts. This
               means that for two routes "/prada/:id" and "/:designer/shoes", the URL 
               "/prada/shoes" will match the first - "/prada/:id".
        
            #. All the parts must be found to match either exactly or a wildcard.
        
            #. It is an error if no routes are found or if more than one route matches.
        
    .. method:: method()

    .. method:: param(key::string, -as::tag =?, -default =?, -selectall::boolean =?)

    .. method:: params()

    .. method:: path()

    .. method:: postParam(key::string, -as::tag =?, -default =?, -selectall::boolean =?)

    .. method:: postParams()

    .. method:: protocol()

    .. method:: queryParam(key::string, -as::tag =?, -default =?, -selectall::boolean =?)

    .. method:: queryParams()

    .. method:: register(controller::string, -routes::staticarray =?)

        This method serves two functions: 1) It registers a controller and its code 
        block into Rope. 2) It registers routes with an existing controller.
        
        If a controller of a given name doesn't exist, then this method expects a block
        to be passed in for it to register the controller. You can register routes at
        the same time you are registering the controller with its code block. If you do
        not pass a code block, then it is assumed that you are registering routes to an
        existing controller and an error will be thrown if that controller does not
        exist.
        
        Examples::
        
            // Good
            rope->register('products') => {
                .... snip ....
            }
            rope->register('product-show', -routes=(:'/product/:id')) => {
                .... snip ....
            }
            rope->register('product-show', -routes=(:'/widget/:id'))
        
            // Bad
            rope->register('new-controller', -routes=(:'/moose/hair/rhino'))
        
    .. method:: render(path::string, ...)

    .. method:: render(view::file, ...)

    .. method:: renderJSON(content::string)

    .. method:: renderJSON(item)

    .. method:: routesSized(size::integer)

    .. method:: sess(key::string)

    .. method:: sess=(rhs::trait_serializable, key::string)

    .. method:: startupCheckNLoad()

    .. method:: status_msg()

    .. method:: status_msg=(status_msg)

    .. method:: statusMsg(-status::boolean =?, -message::boolean =?)

    .. method:: statusMsg(stat::string, msg::string)

    .. method:: statusMsg(stat_msg::pair)

    .. method:: statusMsgError(msg::string)

    .. method:: statusMsgSuccess(msg::string)

    .. method:: statusMsgWarning(msg::string)

    .. method:: urlParam(key::string, -as::tag =?, -default =?, -selectall::boolean =?)

    .. method:: urlParams()

    .. method:: urlParams=(urlParams::array)

    .. method:: user()

    .. method:: user=(rhs::null)

    .. method:: user=(rhs::trait_rope_user)

    .. method:: version()

    .. method:: viewForPath(path::string)

.. method:: rope()

