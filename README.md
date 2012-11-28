Rope
====

Rope is a light-weight framework built for Lasso 9.


Release Notes
-------------

New in version 0.8

- New [rope->user] methods for using the rope session to store a user object
- New [trait\_rope\_user] that user objects stored in [rope->user] need to implement
- New type [trait\_user] that implements [trait\_rope\_user]
- New utility method [rope\_cycle] for rotating between a list of items
- New utility method [rope\_hash] for generating hex encoded hashes
- New utility method [rope\_randomASCII] that generates a random string of visible ASCII characters


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