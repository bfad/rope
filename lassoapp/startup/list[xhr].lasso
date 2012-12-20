    local(dir2list) = dir('/' + string(web_request->queryParam('path') || sys_userStartupPath))
    local(exts) = (
        with item in string(web_request->queryParam('exts'))->split(' ') 
        where #item->size > 0 
        select #item
    )->asStaticArray
    return json_serialize(map(
        "files" = with fp in #dir2list->eachFilePath where #exts->contains(#fp->split('.')->last) select #fp->lastComponent,
        "dirs"  = with dp in #dir2list->eachDirPath  select #dp->removeTrailing('/')&lastComponent
    ))