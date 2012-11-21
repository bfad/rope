Rope
====

Rope is a light-weight framework built for Lasso 9.


Release Notes
-------------

New in version 0.1.4

- New [rope->renderJSON] method sets the HTTP response headers up correctly and will take serialized JSON string or a non-string object and serialize it and then set it to [content\_body]
- Changed the error code for a missing route to 404 (error_code_fileNotFound)
- Bugfix [rope->path] had code trying to set the last element in an array like this [#path->last = ...]


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