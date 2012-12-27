#! /usr/bin/lasso9

not sys_getenv('LASSO9_RETAIN_COMMENTS')
    ? fail('This program requires that Lasso be run with the LASSO9_RETAIN_COMMENTS environment variable set to 1')
    
// Find the absolute path here so we can source files relative to this one
local(path_here) = currentCapture->callsite_file->stripLastComponent
not #path_here->beginsWith('/')
    ? #path_here = io_file_getcwd + '/' + #path_here
not #path_here->endsWith('/')
    ? #path_here->append('/')
#path_here = "/" + #path_here

local(core_libs) = (:
	'rope',
	'rope_user',
	'rope_validator',
	'utilities'
)    
local(libs_path)    = #path_here + `../lassoapp/lib/`
local(lib_ext)      = '.inc'
local(doc_src_path) = #path_here + `source/api/`

    
// How things should be indented
define indentor => '    '

// Procecssing doc comments by stripping the beginning and ending comment parts
// and reformatting the indentation based on the passed indentation level.
define process_doccomment(comment::string, indent_lvl::integer=0) => {
    #comment = #comment->asCopy
    #comment->removeLeading('/**!')&removeTrailing('*/')&removeTrailing('*')
             &removeLeading('\r\n')&removeLeading('\r')&removeLeading('\n')
    
    #comment->size == 0
        ? return
    
    local(ret) = ''
    
    // The first line of the doccomment will tell us how much whitespace is
    // the base - for each line we'll want to remove that amount
    local(num_remove) = 1
    while(#comment->isWhitespace(#num_remove)) => { #num_remove++ }
    #num_remove--
    
    with line in #comment->eachLine do {
        #line->remove(1, #num_remove)
        #ret->append((indentor * #indent_lvl) + #line + '\n')
    }

    return #ret
}    


/*
    Based on my contribution to Knop
    Basic algorithm is to keep track of the number of methods and types currently loaded,
    and then load a library file and process the new methods and types that are loaded.
    However, because Lasso can dynamically load libraries, some of the new methods and
    types are from other libriaries and not defined in the file we just sourcefiled.
    Since all methods and types in rope are properly namespaced, we can filter for those
    methods and types that begin with "rope".
*/

// These variables are used to keep track of what has already been processed
local(types) = map
local(listTypes_index)
local(listTraits_index)
local(listMethods_index)

with lib in #core_libs do {
    #listTraits_index  = sys_listTraits->size
    #listTypes_index   = sys_listTypes->size
    #listMethods_index = sys_listUnboundMethods->size
    sourcefile(file(#libs_path + #lib + #lib_ext), -autoCollect=false)->invoke

    local(doc) = file(#doc_src_path + #lib + `.rst`)
    handle => { #doc->close }
    #doc->openTruncate&writeBytes(bytes(#lib + '\n' + ('=' * #lib->size) + '\n\n'))

    with trait in sys_listTraits
    skip #listTraits_index
    where #trait->asString->beginsWith('trait_rope')
    do {
        #types->insert(#trait->asString)
        
        #doc->writeBytes(bytes('.. class:: ' + #trait + '\n\n'))
        local(comment) = process_doccomment(#trait->doccomment, 1)
        #comment->size > 0
            ? #doc->writeBytes(bytes(#comment))
                
        if(#trait->getType->subtraits->size > 0) => {
            #doc->writeBytes(bytes(indentor + 'Imported Traits:\n\n'))
            with subtrait in #trait->getType->subtraits
            where #subtrait != #trait
            do {
                #doc->writeBytes(bytes((indentor * 2) + '* ' + #subtrait + '\n\n'))
            }
        }
            
        if(#trait->getType->requires->size > 0) => {
            #doc->writeBytes(bytes(indentor + 'Required Methods:\n\n'))
            with method in #trait->getType->requires
            where #method->typeName == #trait
            let output = (#method->asString->remove(1, #trait->asString->size + 2)&)
            do {
                #doc->writeBytes(bytes((indentor * 2) + '* ' + #output + '\n\n'))
            }
        }
        
        with method in #trait->getType->provides
        where #method->typeName == #trait
        let output  = (#method->asString->remove(1, #trait->asString->size + 2)&)
        let comment = process_doccomment(#method->doccomment, 2)
        do {
            #doc->writeBytes(bytes(indentor + '.. method:: ' + #output + '\n\n'))
            #comment->size > 0
                ? #doc->writeBytes(bytes(#comment))
        }
    }
    
    with type in sys_listTypes
    skip #listTypes_index
    where #type->asString->beginsWith(`rope`)
    do {
        #types->insert(#type->asString)
        
        // Deal with thread types having an extra type with "_thread$" tacked on to the end
        // (It should come before the actual type object, so we can steal the doccoment for it)
        if(#type->asString->endsWith(`_thread$`)) => {
            local(thread_type = #type->asString->removeTrailing('_thread$')&)
            tag(#thread_type)->docComment = #type->docComment
        else
            #doc->writeBytes(bytes('.. class:: ' + #type + '\n\n'))
            local(comment) = process_doccomment(#type->doccomment, 1)
            #comment->size > 0
                ? #doc->writeBytes(bytes(#comment))
    
            // Get member methods defined specifically for the type
            with method in #type->getType->listMethods
            where #method->typeName == #type
            where `'` != #method->methodName->asString->get(1)
            let output = (#method->asString->remove(1, #type->asString->size + 2)&)
            let comment = process_doccomment(#method->doccomment, 2)
            order by #output
            do {
                #doc->writeBytes(bytes(indentor + '.. method:: ' + #output + '\n\n'))
                #comment->size > 0
                    ? #doc->writeBytes(bytes(#comment))
            }
        }
    }

    // Process rope methods
    // Don't want to include onCreate methods
    // Also using listMethods_index to track where the new methods start
    with method in sys_listUnboundMethods
    skip #listMethods_index
    let name = #method->methodName->asString
    where #name->beginsWith(`rope`)
    where not #types->contains(#name)
    let comment = process_doccomment(#method->doccomment, 1)
    order by #method->asString
    do {
        #doc->writeBytes(bytes('.. method:: ' + #method + '\n\n'))
        #comment->size > 0
            ? #doc->writeBytes(bytes(#comment))
    }
}


file(#doc_src_path + `index.rst`)->doWithClose => {
    local(f) = #1
    
    #f->openTruncate&writeBytes(bytes("\
.. Rope documentation master file, created by
   gen_api on " + date->format(`%a %b %-d %H:%M:%S %Y.`) + "
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Welcome to Rope's API Reference!
================================

Contents:

.. toctree::
   :maxdepth: 2
   
"
    ))
    with lib in #core_libs do #f->writeBytes(bytes('   ' + #lib + '\n'))
}