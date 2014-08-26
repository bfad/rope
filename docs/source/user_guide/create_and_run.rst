Creating and Running a Rope App
===============================
In this section, you will learn how to create a Rope application and run the 
built-in web server for development. All examples will assume that you have
successfully installed Rope and that you have set up your ``PATH`` environment
variable so that you don't have to specify the full path to the Rope script.


Creating a New Project
----------------------
To create a new Rope application, open a terminal and change to the directory
you want to create the new project in and issue the ``rope new <NAME>`` command
or the short-cut ``rope n <NAME>`` command. Ex::

   $> cd ~/projects/
   $> rope new do_list

The above example will create a directory named "do_list" in the projects
directory in your home folder. That new directory will contain the basic file
and directory layout for your Rope application.

Project Directory Layout
^^^^^^^^^^^^^^^^^^^^^^^^
Rope is meant to be very flexible, so it's default folder structure doesn't try
and impose any architectural model on your project. Below is a list of the
folders that are created and a description of what they are used for.

lasso_home/
   This is the Lasso instance home folder for your application. When you run the
   built-in web server, it sets this as your LASSO9_HOME environment variable.

views/
   This is a special folder used with ``rope->include`` that allows for it to be
   the default root instead of your webroot. This allows your webroot to contain
   no Lasso code if you desire.

webapp/
   This is the directory where you will probably keep all your business logic.
   This folder is actually symbolically linked as the LassoStartup folder in the
   lasso_home directory.

webroot/
   This is the webroot for your web application. Stick all your javascript, css,
   and other assets in here.


Running the Built-in Web Server
-------------------------------
To run your Rope application, change your working directory to the top-level of
your Rope application and then issue the ``rope server`` or ``rope s`` command.
Ex::

   $> cd ~/projects/do_list
   $> rope s

This starts the built-in web server and loads your Rope application. While this
web server is suitable for development work, you should look into deploying your
application with a dedicated web server such as Apache or Nginx. (See the
instructions on :doc:`deploying Rope applications <deploy>` for details.)
