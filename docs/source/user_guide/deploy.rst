Deploying Rope Apps
===================
Every web application is different, so there's no way we can cover everything
about deploying your Rope application. The instructions below will walk you
through deploying a basic Rope application using a modern version of Apache and
the Lasso Instance Manager.


Deploying with Apache & LassoIM
-------------------------------
This deployment guide assumes that you can install Apache, Lasso, and the Lasso
Instance Manager on your server OS of choice. It assumes a fresh installation
with no additional setup. It assumes you can take the provided Apache virtual
host setup and adapt it to your system's Apache configuration.

#. Copy your Rope application directory to your deployment server. For the sake
   of this example, we will have a Rope application named "example" that we copy
   to the ``/var/www/`` directory. (So the application directory path is 
   ``/var/www/example``.)

#. Open a web browser and navigate to the server's instance manager
   configuration page (http://example.com/lasso9/instancemanager).

#. Stop (disable) the default instance that got created.

#. From the command-line, remove the instance's home folder and symbolically
   link the "lasso_home" directory in your Rope application to where the 
   instance's home directory used to be::

   $> sudo rm -rf /var/lasso/instances/default
   $> sudo ln -s /var/www/example/lasso_home /var/lasso/instances/default

#. Back in the browser, navigate to the environment variable and add the
   following environment variable: ``ROPE_ENV=production``.

#. Start the instance.

#. Make your Apache virtual host site configuration look something like the
   following::

      <VirtualHost *:80>
         ServerAdmin webmaster@localhost

         DocumentRoot /var/www/example/webroot
         <Directory />
            Options FollowSymLinks
            AllowOverride None
         </Directory>
         <Directory /var/www/example/webroot/>
            Options FollowSymLinks
            AllowOverride None
            Order allow,deny
            allow from all
         </Directory>

         RewriteEngine on
         RewriteCond %{REQUEST_URI}  !^/lasso9/.*$
         RewriteCond %{DOCUMENT_ROOT}%{REQUEST_FILENAME} !-f
         RewriteRule ^(.*)$ /lasso9/rope/url_handler.lasso [L,NS,H=lasso9-handler,E=LASSOSERVER_APP_PREFIX:/lasso9/rope]
      </VirtualHost>
   
#. Once your Apache settings are setup, reload Apache and your app should now be
   running.