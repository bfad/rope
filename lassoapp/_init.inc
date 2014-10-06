handle_error => {log_critical(error_msg + '\n' + error_stack)}
local(lib) = (:
    '/lib/rope.inc',
    '/lib/rope_user.inc',
    '/lib/utilities.inc',
    '/lib/rope_validator.inc'
)
with f in #lib do lassoapp_include_current(#f)

define rope_webappPath => sys_homePath + '/rope_webapp/'

rope->register(`root`, -routes=(:'/')) => { redirect_url('/lasso9/rope') }

protect => {\http_server_connection_handler}
define http_server_connection_handler->invoke(con::http_server_web_connection) => {
    protect => {
        handle_failure => {
            stdoutnl(error_msg)
            stdoutnl(error_stack)
        }

        web_request  = web_request_impl(#con) // make it available for the thread
        web_response = web_response_impl(#con)

        local(path)     = #con->requestParams->get(::PATH_INFO)->asString
        local(ext)      = #path->splitExtension->second
        local(document) = file(.documentRoot + #path)
        //local(is_lasso) = (#path->beginsWith(http_server_apps_path) or not #document->exists or (#ext->size and .scriptExtensions >> #ext))
        local(is_lasso) = (#path->beginsWith(`/lasso9/`) or (#ext->size and .scriptExtensions >> #ext))
    
        if(not #is_lasso and #document->exists and not #document->isDir) => {
            web_response->setStatus(200, 'OK')
            .sendFile(#document, #ext)
        // pass off non-existing files to rope
        else(not #is_lasso)
            #is_lasso = true
            #con->requestParams->insert(::LASSOSERVER_APP_PREFIX=`/lasso9/rope`)
            #con->requestParams->insert(::PATH_INFO=`/rope/url_handler.lasso`)
        }
        if(#is_lasso) => {
            #con->requestParams->insert(::LASSOSERVER_DOCUMENT_ROOT=.documentRoot)
            web_response->respond
        }
    }
    // keep alive requests use the same thread, so reset thread-local data
    web_request  = null
    web_response = null
    document     = null
    var(_rope_)  = null
    var(_lassoapp_include_stack_) = null
    var(__HTML_REPLY__) = ''
}