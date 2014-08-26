Configuration
=============
When you create a new Rope application, a file named "_config.inc" is created in
the "webapp" directory. This file has code that looks like the following::

   not rope->configLoaded?
   rope->config(
       `app-name` = `MyApp`
   )

This code checks to see if the configuration has been loaded, and loads it if it
has not using ``rope->config``.  The ``rope->config`` method takes an arbitrary
number of pairs whose first element is a string specifying the name of the
configuration setting and whose second element is the value to assign to it. In
the code above, just the "app-name" setting is being set to the name of your
application.


Configuration Settings
----------------------
The following are a list of Rope configuration settings, their default
values, and what they do:

app-name
   *Default Value:* "UnNamed"

   This is the name of your application - choose it wisely!

sessions-enabled
   *Default Value:* true
   
   If enabled, allows for using ``rope->sess`` to store and retrieve session
   values. This value must be set to ``true`` to use ``rope->user``.

sessions-expires
   *Default Value:* 1440
   
   This specifies the timeout value for sessions in minutes.