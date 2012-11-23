handle_error => {log_critical(error_msg + '\n' + error_stack)}
lassoapp_include_current('/rope.inc')

rope->register(`root`, -routes=(:'/')) => { redirect_url('/lasso9/rope') }