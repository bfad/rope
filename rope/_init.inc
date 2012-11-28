handle_error => {log_critical(error_msg + '\n' + error_stack)}
lassoapp_include_current('/rope.inc')
lassoapp_include_current('/root.inc')

// Utilities
define  string->encodeUrl(strict::boolean=true) => bytes(self)->encodeUrl(#strict)
define integer->encodeUrl(strict::boolean=true) => bytes(self->asString)