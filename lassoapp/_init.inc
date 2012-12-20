handle_error => {log_critical(error_msg + '\n' + error_stack)}
local(lib) = (:
    '/lib/rope.inc',
    '/lib/rope_user.inc',
    '/lib/utilities.inc',
    '/lib/rope_validator.inc'
)
with f in #lib do lassoapp_include_current(#f)

lassoapp_include_current('/root.inc')