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
