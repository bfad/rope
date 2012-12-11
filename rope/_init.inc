handle_error => {log_critical(error_msg + '\n' + error_stack)}
local(lib) = (:
    '/lib/rope.inc',
    '/lib/rope_user.inc',
    '/lib/utilities.inc'
)
with f in #lib do lassoapp_include_current(#f)

rope->register(`root`, -routes=(:'/')) => { redirect_url('/lasso9/rope') }