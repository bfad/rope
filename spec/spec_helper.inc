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

    import_folder(dir(#path_here + '../src/'))
    
    var(_rope_loaded) = true
}