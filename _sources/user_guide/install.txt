Installing Rope
===============

Rope consists of a LassoApp and a command-line script that must be installed
relative to your ``LASSO9_MASTER_HOME`` environment variable or
``/var/lasso/home/`` if no ``LASSO9_MASTER_HOME`` has been defined. The command-
line script must be installed at ``LASSO9_MASTER_HOME/bin/`` while the LassoApp
can be installed at ``LASSO9_MASTER_HOME/LassoApps/`` if you want it available
for every instance of Lasso, or at ``LASSO9_MASTER_HOME/LassoApps-disabled/`` if
you want Rope to copy the LassoApp into each new Rope application. (If you start
with it installed for all Lasso processes and decide you want to change it
later, then don't forget to copy it to each projects' ``lasso_home/LassoApps``
directory.)

In the steps below, we will be assuming that you will be installing Rope into
the ``/var/lasso/home/`` directory:

#. Download the binaries for your platform:
   `Bitbucket Download Page <https://bitbucket.org/bfad/rope/downloads>`_

#. Decompress and untar the downloaded file::

   $> tar -zxvf /path/to/downloaded_file.tar.gz
  
#. Install the command-line script (after first making the directory)::

   $> mkdir /var/lasso/home/bin/
   $> mv /path/to/rope/rope /var/lasso/home/bin/

#. If you want the Rope LassoApp to be installed with each Rope application::

   $> mkdir /var/lasso/home/LassoApps-disabled/
   $> mv /path/to/rope/rope.lassoapp /var/lasso/home/LassoApps-disabled/

#. If you want to have the Rope LassoApp available for every instance::

   $> mv /path/to/rope/rope.lassoapp /var/lasso/home/LassoApps/