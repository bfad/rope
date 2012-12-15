<?=
    local(scriptExts) = web_request->param('exts')->split(' ')->asStaticArray
    dir_import(dir('/' + sys_userStartupPath), #scriptExts)
?>