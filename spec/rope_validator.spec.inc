if(not var_defined('_rope_loaded')) => {
    // This should let us run this anywhere and still properly import the file
    local(path_here) = currentCapture->callsite_file->stripLastComponent
    not #path_here->beginsWith('/')? #path_here = io_file_getcwd + '/' + #path_here
    not #path_here->endsWith('/') ? #path_here += '/'
    local(f) = file(#path_here + 'spec_helper.inc')
    sourcefile(#f->readString, #f->path, false, false)->invoke
}

describe(::rope_validator) => {
    describe(`-> onCreate`) => {
        it(`takes in a map of validations and an arbitrary amount of keywords name/value`) => {
            expect->error(error_code_noError, error_msg_noError) => {
                rope_validator(map, -moose='hair')
                rope_validator(map, -moose='hair', -foo='bar')
            }
        }
        it(`takes in a map of validations and an arbitrary amount of pairs`) => {
            expect->error(error_code_noError, error_msg_noError) => {
                rope_validator(map, 'moose'='hair')
                rope_validator(map, 'moose'='hair', 'foo'='bar')
            }
        }
        it(`throws a "method not found" error if not passed anything to the rest parameters`) => {
            expect->error(error_code_methodNotFound, error_msg_methodNotFound + `: Candidtates are rope_validator(validations::map, ...)`) => {
                rope_validator(map)
            }
        }
        it(`throws an invalid parameter error if an element in the rest params is not a pair or keyword`) => {
            expect->error(error_code_invalidParameter, error_msg_invalidParameter) => {
                rope_validator(map, 2)
            }
            expect->error(error_code_invalidParameter, error_msg_invalidParameter) => {
                rope_validator(map, -moose=3, 'hair')
            }
        }
        it(`sets the values data member to a map of the keywords or pairs passed in and the validations data method to the map passed in`) => {
            local(validations) = map('moose' = map)
            local(test_value) = map('foo'='bar', 'bah'='humbugh')
            local(validator) = rope_validator(#validations, -foo='bar', "bah"="humbugh")
            
            // For some reason, just comparing the maps isn't working right, so doing it manually
            expect(#validations->size, #validator->validations->size)
            
            with key in #validator->validations->keys
            do expect(#validations->keys->contains(#key))
            
            with value in #validator->validations->values
            do expect(#validations->values->contains(#value))
            
            expect(#test_value->size, #validator->values->size)
            
            with key in #validator->values->keys
            do expect(#test_value->keys->contains(#key))
            
            with value in #validator->values->values
            do expect(#test_value->values->contains(#value))
        }
    }
}