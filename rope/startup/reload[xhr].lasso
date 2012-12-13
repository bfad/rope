<?=
    local(file2reload) = file('/' + string(web_request->queryParam('path')))
    
    if(not #file2reload->exists) => {
        web_response->setStatus(400, 'Bad Request')
        return "Specified file does not exist."
    }
    
    sourcefile(#file2reload->readString, #file2reload->path, false, false)->invoke
?>