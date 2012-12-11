
    local(dir2list) = dir('/' + string(web_request->queryParam('path') || sys_userStartupPath))

    return json_serialize(map(
        "files" = with fp in #dir2list->eachFilePath select #fp->lastComponent,
        "dirs"  = with dp in #dir2list->eachDirPath  select #dp->removeTrailing('/')&lastComponent
    ))
