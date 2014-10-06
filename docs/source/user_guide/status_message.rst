Status Messages
===============
The various ``rope->statusMsg...`` methods can be used to temporarily store a
a message that you can then display in any template however you want. This part
of the Rope framework needs some work or some re-thinking, so consider these
methods deprecated.

A status message consists of a status and a message. A status message can be set
using the ``rope->statusMsg(stat::string, msg::string)`` setter and retrieved as
a map with with a `status` key and a `message` key. with the ``rope->statusMsg``
getter method. Just the status can be retrieved by calling
``rope->statusMsg(-status)``, and just the message can be retrieved by calling
``rope->statusMsg(-message)``.

On top of that, there are a few convenience methods that require a message, but
will automatically set the status portion. The ``rope->statusMsgSuccess`` method
will set the status to "success", the ``rope->statusMsgWarning`` method will set
the status to "warning", and the ``rope->statusMsgError`` method will set the
status to "error".