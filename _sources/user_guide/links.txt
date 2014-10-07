Rope Route Links
================
Registered routes can be referenced by their names to create URL paths those
routes will respond to by using the ``rope->link`` method. Using this method
allows for simple changes to URL paths to be easily made in one place. And even
if the need arises to make a change in every place a particular path is used,
using the method allows for you to easily find all those places.

Because of the complexity of registering routes, the ``rope->link`` method has
several different implementations. The simplest route registrations can use the
simplest ``rope->link`` API. In the example below, the route only has one URL
path associated with it and that path doesn't contain any URL parameters.
::

   rope->register(`home-dashboard`, -routes=(:'/home/dashboard')) => {}
   
   rope->link(`home-dashboard`)
   // => /home/dashboard

If you have multiple URL paths registered to the route, you can specify which
path you want by providing the index of the path in the array. (By default, the
first path will be used if you don't specify an index.)
::

   rope->register(`login`, -routes=(:'/login', '/logout')) => {}

   rope->link(`login`)
   // => /login

   rope->link(`login`, 2)
   // => /logout

If the URL path of the route has any wild-card parameters, values for those
parameters must be specified in an array or staticarray::

   rope->register(`user-photo`, -routes=(:'/user/:id/photo/:photo_id')) => {}

   rope->link(`user-photo`, (:5, 42))
   // => /user/5/photo/42

If the route contains both multiple URL paths and wild-card parameters then both
the index of the URL path and the values for the wild-cards must be specified::

   rope->register(`phone-add-edit`, -routes=(:'/phones/new', '/phone/:id')) => {}

   rope->link(`phone-add-edit`, 2, (:12))
   / => /phone/12

