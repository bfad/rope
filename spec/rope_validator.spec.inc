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
    
    
    describe(`-> validate(key::string)`) => {
        //it(`returns true when the data for the key is valid`)
        //it(`returns false when the data for the key is invalid`)
        context('when there are no validations') => {
            it(`returns true when the specified key isn't found in the validations`) => {
                expect(rope_validator(map, -foo=2)->validate('breath'))
            }
            it(`returns true when the validations for the specified key is an empty map`) => {
                expect(rope_validator(map('foo'=map), -foo=2)->validate('foo'))
            }
            it(`returns true when the validations for the specified key is an unknown validation type`) => {
                expect(rope_validator(map('foo'=map('r12ndone'=2)), -foo=2)->validate('foo'))
            }
        }
        
        context('when there is a type validation') => {
            local(validations) = map(
                'moose' = map('type'=::string),
                'rhino' = map('type'=(: ::string, ::integer)),
            )
            it(`returns true when the specified value is of the type specified`) => {
                expect(rope_validator(#validations, -moose='')->validate('moose'))
            }
            it(`returns true when the specified value is of one of the specified types`) => {
                expect(rope_validator(#validations, -rhino=42)->validate('rhino'))
            }
            it(`returns false when the specified value is not of the type specified`) => {
                expect(not rope_validator(#validations, -moose=42)->validate('moose'))
            }
            it(`returns false when the specified value is not of one of the specified types`) => {
                expect(not rope_validator(#validations, -rhino=date)->validate('rhino'))
            }
        }
        
        context('when there is a min validation') => {
            local(validations) = map('rhino' = map('min'=2))
            
            context(`the value type is *not* a trait_finite`) => {
                it(`returns true when the value is >= the minimum specified`) => {
                    expect(rope_validator(#validations, -rhino=42)->validate('rhino'))
                    expect(rope_validator(#validations, -rhino=2)->validate('rhino'))
                }
                it(`returns false when the value is < the minimum specified`) => {
                    expect(not rope_validator(#validations, -rhino=1)->validate('rhino'))
                }
            }
            context(`the value type is a trait_finite`) => {
                it(`returns true when the size of the value is >= the minimum specified`) => {
                    expect(rope_validator(#validations, -rhino='42')->validate('rhino'))
                    expect(rope_validator(#validations, -rhino='two')->validate('rhino'))
                }
                it(`returns false when the size of the value is < the minimum specified`) => {
                    expect(not rope_validator(#validations, -rhino='1')->validate('rhino'))
                }
            }
            
            it(`returns true without further checks if the type is null and null is an allowed type`) => {
                local(validations) = map('rhino' = map('type'=(: ::null, ::integer), 'min'=2))
                expect(rope_validator(#validations, -rhino=null)->validate('rhino'))
            }
            it(`returns true without further checks if the type is void and void is an allowed type`) => {
                local(validations) = map('rhino' = map('type'=(: ::void, ::integer), 'min'=2))
                expect(rope_validator(#validations, -rhino=void)->validate('rhino'))
            }
        }
        
        context('when there is a max validation') => {
            local(validations) = map('rhino' = map('max'=2))
            
            context(`the value type is *not* a trait_finite`) => {
                it(`returns true when the value is <= the maximum specified`) => {
                    expect(rope_validator(#validations, -rhino=0)->validate('rhino'))
                    expect(rope_validator(#validations, -rhino=2)->validate('rhino'))
                }
                it(`returns false when the value is > the maximum specified`) => {
                    expect(not rope_validator(#validations, -rhino=42)->validate('rhino'))
                }
            }
            context(`the value type is a trait_finite`) => {
                it(`returns true when the size of the value is <= the maximum specified`) => {
                    expect(rope_validator(#validations, -rhino='42')->validate('rhino'))
                    expect(rope_validator(#validations, -rhino='')->validate('rhino'))
                }
                it(`returns false when the size of the value is > the maximum specified`) => {
                    expect(not rope_validator(#validations, -rhino='rhino')->validate('rhino'))
                }
            }
            
            it(`returns true without further checks if the type is null and null is an allowed type`) => {
                local(validations) = map('rhino' = map('type'=(: ::null, ::integer), 'max'=2))
                expect(rope_validator(#validations, -rhino=null)->validate('rhino'))
            }
            it(`returns true without further checks if the type is void and void is an allowed type`) => {
                local(validations) = map('rhino' = map('type'=(: ::void, ::integer), 'max'=2))
                expect(rope_validator(#validations, -rhino=void)->validate('rhino'))
            }
        }
        
        context('when there is a range validation') => {
            local(validations) = map('rhino' = map('range'=2 to 4))
            
            context(`the value type is *not* a trait_finite`) => {
                it(`returns true when the value is >= the minimum specified`) => {
                    expect(rope_validator(#validations, -rhino=3)->validate('rhino'))
                    expect(rope_validator(#validations, -rhino=2)->validate('rhino'))
                }
                it(`returns false when the value is < the minimum specified`) => {
                    expect(not rope_validator(#validations, -rhino=1)->validate('rhino'))
                }
                it(`returns true when the value is <= the maximum specified`) => {
                    expect(rope_validator(#validations, -rhino=4)->validate('rhino'))
                    expect(rope_validator(#validations, -rhino=3)->validate('rhino'))
                }
                it(`returns false when the value is > the maximum specified`) => {
                    expect(not rope_validator(#validations, -rhino=42)->validate('rhino'))
                }
            }
            context(`the value type is a trait_finite`) => {
                it(`returns true when the size of the value is >= the minimum specified`) => {
                    expect(rope_validator(#validations, -rhino='42')->validate('rhino'))
                    expect(rope_validator(#validations, -rhino='two')->validate('rhino'))
                }
                it(`returns false when the size of the value is < the minimum specified`) => {
                    expect(not rope_validator(#validations, -rhino='1')->validate('rhino'))
                }
                it(`returns true when the size of the value is <= the maximum specified`) => {
                    expect(rope_validator(#validations, -rhino='two')->validate('rhino'))
                    expect(rope_validator(#validations, -rhino='mare')->validate('rhino'))
                }
                it(`returns false when the size of the value is > the maximum specified`) => {
                    expect(not rope_validator(#validations, -rhino='rhino')->validate('rhino'))
                }
            }
            
            it(`returns true without further checks if the type is null and null is an allowed type`) => {
                local(validations) = map('rhino' = map('type'=(: ::null, ::integer), 'range'=2 to 4))
                expect(rope_validator(#validations, -rhino=null)->validate('rhino'))
            }
            it(`returns true without further checks if the type is void and void is an allowed type`) => {
                local(validations) = map('rhino' = map('type'=(: ::void, ::integer), 'range'=2 to 4))
                expect(rope_validator(#validations, -rhino=void)->validate('rhino'))
            }
        }
        
        context('when there is a custom validation') => {
            it(`returns false when the block returns nothing`) => {
                local(validations) = map('rhino' = map('custom' = _() => {} ))
                
                expect(not rope_validator(#validations, -rhino=1)->validate('rhino'))
            }
            it(`returns true when the block returns true`) => {
                local(validations) = map('rhino' = map('custom' = _() => { return true } ))
                
                expect(rope_validator(#validations, -rhino=1)->validate('rhino'))
            }
            it(`returns false when the block returns false`) => {
                local(validations) = map('rhino' = map('custom' = _() => { return false } ))
                
                expect(not rope_validator(#validations, -rhino=1)->validate('rhino'))
            }
            it(`passes the value as the first parameter when invoking the given block`) => {
                local(validations) = map('rhino' = map('custom' = _() => { return (#1 == 42) } ))
                
                expect(rope_validator(#validations, -rhino=42)->validate('rhino'))
            }
            it(`passes the values map as the second parameter when invoking the given block`) => {
                local(validations) = map('rhino' = map('custom' = _() => { return (#1 == #2->find('rhino')) } ))
                
                expect(rope_validator(#validations, -rhino=42)->validate('rhino'))
            }
            
            it(`returns true without further checks if the type is null and null is an allowed type`) => {
                local(validations) = map('rhino' = map('type'=null, 'custom' = _() => { return false } ))
                
                expect(rope_validator(#validations, -rhino=null)->validate('rhino'))
            }
            it(`returns true without further checks if the type is void and void is an allowed type`) => {
                local(validations) = map('rhino' = map('type'=void, 'custom' = _() => { return false } ))
                
                expect(rope_validator(#validations, -rhino=void)->validate('rhino'))
            }
        }
    }
    
    
    describe(`-> allValid`) => {
        local(validations) = map(
            'moose' = map('type' = ::string),
            'rhino' = map('type' = ::integer)
        )
        
        it(`returns true if all data elements are valid`) => {
            expect(rope_validator(#validations, -moose='hair', -rhino=42)->allValid)
        }
        it(`returns false if any data element is invalid`) => {
            expect(not rope_validator(#validations, -moose='hair', -rhino='horn')->allValid)
        }
        it(`sets the failed data member to an empty array if all valid`) => {
            local(validator) = rope_validator(#validations, -moose='hair', -rhino=42)
            
            expect(null, #validator->failed)
            expect(#validator->allValid)
            expect(::staticarray, #validator->failed->type)
            expect(0, #validator->failed->size)
        }
        it(`sets the failed data member to an array of those keys that were found to be invalid`) => {
            local(validator) = rope_validator(#validations, -moose='hair', -rhino='horn')
            
            expect(null, #validator->failed)
            expect(not #validator->allValid)
            expect(::staticarray, #validator->failed->type)
            expect(1, #validator->failed->size)
            expect('rhino', #validator->failed->get(1))
        }
        it(`doesn't run through the validations if it has already done so`) => {
            local(validator) = rope_validator(map('rhino'=map('custom'=_() => {yield 1; yield 2; yield 3;} )), -rhino=2)
            
            #validator->allValid
            #validator->allValid
            expect(2, #validator->validations->find('rhino')->find('custom')->invoke)
        }
    }
    
    
    describe(`-> hasAllValidRun`) => {
        it(`returns false if the allValid method hasn't run`) => {
            expect(not rope_validator(map, -foo=1)->hasAllValidRun)
        }
        it(`returns true if the allValid method has run`) => {
            local(validator) = rope_validator(map, -foo=1)

            #validator->allValid
            expect(#validator->hasAllValidRun)
        }
    }
}