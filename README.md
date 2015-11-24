Rope
====

[![Gitter](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/bfad/rope?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

Rope is a light-weight web application framework built for Lasso 9. Rope was
designed to make it easy for developers to create web applications with clean
URLs while promoting best-practices such as keeping presentation code and logic
code separated. With Rope, developers can easily register blocks of code to run
when certain URLs are requested, programmatically generate these URL paths in
their templates, pass local variables into included code files, create
validations for custom types, and more.


Release Notes
-------------

New in version 1.0.2

- Bugfix: You should no longer see any session links - it should be set to only use cookies

***

New in version 1.0.1

- Bugfix: The automatic reloader works again - it had been looking in "$LASSO9_HOME/LassoStartup/" and it's been changed to "$LASSO9_HOME/rope_webapp/"

***

New in version 1.0

- Deprecated: The [rope->user] capabilities will be removed in a future version of rope
- The code that lists files and allows them to be reload has been overhauled and improved
- Added a command-line tool to create projects and run a development web server - this has gone through a couple of iterations
- Added trait_rope_objectValidator and a bunch of validators to aid in validating object values
- Now there is some basic documentation in the form of an user guide and api guide
- Can now specify which environment rope is running in using the ROPE_ENV shell variable (specifying production prevents automatic reloads)
- Created the [rope->env] to return the value of the ROPE_ENV environment variable - defaults to "dev"
- The [rope->param] methods can now specify a method to call to convert the value to the desired type
- Added the [stringOrNull], [integerOrNull], [decimalOrNull], and [dateOrNull] convenience methods to be used as method parameters to [rope->param] methods
- Changed the default data type to be returned from the [rope->param] methods from [string] to [stringOrNull]
- The [rope->param] methods now take a parameter that can specify the default value to use when the specified parameter does not exist
- Added convenience methods to store and retrieve status messages
- New [rope->link] method allows for creating URL paths from controller names
- Add utility [rope_randomAlphaNumeric] to generate random strings of letters and numbers
- Performance improvements for [rope->path] and [rope->extension]
- URL requests that end with a trailing slash will now match controller routes whose paths do not have the trailing slash
- New [rope->fileUploads] method that returns an array with a map for each file uploaded - the keys of the maps correspond to the first value of the pairs returned using Lasso's built-in [web_request->fileUploads] method
- The [dir_import] utility method now sorts the folders and files by name


License
-------

Copyright 2014 Bradley Lindsay

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

>    [http://www.apache.org/licenses/LICENSE-2.0](http://www.apache.org/licenses/LICENSE-2.0)

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.