Rope
====

Rope is a light-weight framework built for Lasso 9.


Release Notes
-------------

New in version 0.9

- New fallback controller that can be created for unmatched paths
- Now [rope->method] take into account query or post param named _method (since browsers forms can only do GET & POST)
- New autoreloading feature that checks for changes / additions to files in LassoStartup for each request and loads the change or added files
- Now [rope->include] can be given relative paths to other view files
- Added doc comments that will eventually be used when autogenerating the API documentation
- Bugfix: Routes assigned to a controller are now removed when the controller is deregistered


License
-------

Copyright 2012 Bradley Lindsay

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

>    [http://www.apache.org/licenses/LICENSE-2.0](http://www.apache.org/licenses/LICENSE-2.0)

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.