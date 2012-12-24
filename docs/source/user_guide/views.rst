Rendering Views
===============
The Rope framework encourages separating your view code from your controller
code. Your controller code should be placed in the "webapp" directory while the
view code should be placed in the "views" directory. To assist your controllers
in loading the view code in response to a request, you can use the
``rope->render`` method. To do this, pass a string starting specifying the path
to the view code you wish to render. The path should start with a "/" which
represents the "views" directory. You can pass values / setup local variables
for the view code by passing the method keyword parameters - the key of which
will be the name of a local variable whose value is set to the value of the
keyword parameter.

Example::

	// webapp/my_controller.inc
	rope->register(`hello-name`, -routes(:'/:name')) => {
	    rope->render('/hello.lasso', -name=rope->param('name'))
	}
	
	// views/hello.lasso
	<!DOCTYPE html>
	<html>
	  <head></head>
	  <body>
	    <h1>Hello, [#name]</h1>
	  </body>
	</html>

In this example, the controller calls the view code in ``views/hello.lasso`` and
by passing the keyword parameter ``-name`` it sets up a local variable named
"name" for the view code to use.


Including a View from a View
----------------------------
It is possible to render views inside each other. This is done with the
``rope->include`` method which, like the ``rope->render`` method takes a string
specifying the path to the view to be included. It can also take keyword
parameters to setup local variables for use.

Example::

	// webapp/my_controller.inc
	rope->register(`hello-name`, -routes(:'/')) => {
	    rope->render('/home.lasso')
	}
	
	// views/home.lasso
	<!DOCTYPE html>
	<html>
	  <head>
	    <title>Home</title>
	    [rope->include('/_helpers/html_head.lasso', -jqueryUI=true)]
	  </head>
	  <body>
	    <h1>Hello, World!</h1>
	  </body>
	</html>
	
	// views/_helpers/html_head.lasso
	<script src="http://code.jquery.com/jquery-latest.js"></script>
	[if(#jqueryUI)]
	  <script src="http://code.jquery.com/ui/1.9.2/jquery-ui.js"></script>
	[/if]