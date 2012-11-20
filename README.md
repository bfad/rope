Rope
====

Rope is a light-weight framework built for Lasso 9.


Release Notes
-------------

New in version 0.1.1

- New Method [rope->render] allows for easily rendering template files
- When [rope->render] is passed a path it takes it as a relative path and finds the template in /path/to/webroot/../views/my\_template.lasso
- Additionally, [rope->render] can also take named parameters to be passed in as local variables to the template


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