if(not lasso_tagExists(::import_folder)) => {
    define import_folder(d::dir) => {
        with f in #d->eachFile do => {
            if(#f->path->endsWith('.inc') or #f->path->endsWith('.lasso')) => {
                import_file(#f)
            }
        }
        with f in #d->eachDir  do => { import_folder(#f) }
    }
    define import_file(f::file) => {
        sourcefile(#f->readString, #f->path, false, false)->invoke
    }
}
if(not var_defined(`_rope_loaded`)) => {
    // This should let us run this anywhere and still properly import the files
    local(path_here) = {}->callsite_file->stripLastComponent
    not #path_here->beginsWith('/')? #path_here = io_file_getcwd + '/' + #path_here
    not #path_here->endsWith('/') ? #path_here += '/'

    import_folder(dir(#path_here + '../rope/lib/'))
    
    var(_rope_loaded) = true
}

define mock => type {
    data private methods::map
    
    public onCreate(...) => {
        .`methods` = map
        with item in #rest do .`methods`->insert(#item)
    }
    
    public setMethod(p::pair) => {
        .`methods`->insert(#p)
    }
    public setMethod   => .`methods`->get(`setMethod`)
    public methods     => .`methods`->get(`methods`)
    public _unknowntag => .`methods`->get(method_name->asString)
}

define web_response_mock => type {parent mock 
    data private headers::map = map
        
    public onCreate(...) => ..onCreate(:#rest) 
    public rawContent=(rhs::string) => {
        ..setMethod('rawContent'=#rhs)
    }
    public header(key::string) => .'headers'->find(#key)
    public replaceHeader(elm::pair) => .'headers'->insert(#elm)
}

define new_fake_conn => {
    local(con)   = http_server_web_connection(net_tcp)
    web_request  = web_request_impl(#con)
    web_response = web_response_impl(#con)
}
// For sessions
protect => {\http_server_web_connection}
define http_server_web_connection->requestParams => var(_req_params) || $_req_params := map
define http_server_web_connection->stdin         => var(_req_stdin)  || $_req_stdin  := ``
session_initialize
new_fake_conn

// rope_thread stuff
define rope_thread->configLoadedFalse => {.`configLoaded` = false}

define rope_impl->absPath(path::string) => .absolutePath(#path)