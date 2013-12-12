if(not var_defined('_rope_loaded')) => {
    // This should let us run this anywhere and still properly import the file
    local(path_here) = currentCapture->callsite_file->stripLastComponent
    not #path_here->beginsWith('/')? #path_here = io_file_getcwd + '/' + #path_here
    not #path_here->endsWith('/') ? #path_here += '/'
    local(f) = file(#path_here + 'spec_helper.inc')
    sourcefile(#f->readString, #f->path, false, false)->invoke
}


// The type to be used for stuff
define test_typeValidator => type {
    trait { import trait_rope_objectValidator }

    data public ropeValidations = map(
        ::fail1 = { return false },
        ::fail2 = { return 'bad' },
        ::pass1 = { return true  },
        ::pass2 = { return       }
    )

    public fail1 => `fail1`
    public fail2 => `fail2`
    public pass1 => `pass1`
    public pass2 => `pass2`
}


describe(::trait_rope_objectValidator) => {
    describe(`-> ropeValidate`) => {
        context(`passed a string or a tag`) => {
            it(`returns the result of running the specified validation`) => {
                expect(false, test_typeValidator->ropeValidate(`fail1`))
                expect('bad', test_typeValidator->ropeValidate(::fail2))
                expect(true , test_typeValidator->ropeValidate(`pass1`))
                expect(void , test_typeValidator->ropeValidate(::pass2))
            }

            it(`returns void if the specified validation does not exist`) => {
                expect(void, test_typeValidator->ropeValidate(`_not_here_`))
                expect(void, test_typeValidator->ropeValidate(::_not_here_))
            }

            it(`allows memberstreams to be a validator`) => {
                define foo_test_valid_stuff(p) => `hello, ` + #p

                local(test) = test_typeValidator
                #test->ropeValidations = map(::fail1=\foo_test_valid_stuff)

                expect(`hello, fail1`, #test->ropeValidate(::fail1))
            }
        }

        it(`runs through all the validations and returns an array of the fields that failed`) => {
            local(failed) = test_typeValidator->ropeValidate
            local(expect) = map(`fail1`=`This field is invalid`, `fail2`=`bad`)
            
            expect(#expect->keys  , #failed->keys)
            expect(#expect->values, #failed->values)
        }

        context(`passed an array of keys to validate`) => {
            it(`throws an error if there are no items in the list`) => {
                expect->error => { test_typeValidator->ropeValidate((:)) }
            }
            it(`only validates the specified the specified keys`) => {
                local(result) = test_typeValidator->ropeValidate((: ::fail2))
                local(expect) = map(`fail2`=`bad`)

                expect(#expect->keys  , #result->keys)
                expect(#expect->values, #result->values)
            }
        }
    }


    describe(`-> ropeIsValid`) => {
        it(`returns true if the string-specified validation returns true or void`) => {
            local(test) = test_typeValidator
            #test->ropeValidations = map(
                ::pass1 = { return true },
                ::pass2 = { return      }
            )

            expect(true, #test->ropeIsValid(`pass1`))
            expect(true, #test->ropeIsValid(`pass2`))
        }
        it(`returns false if the string-specfied validation returns string or false`) => {
            local(test) = test_typeValidator
            #test->ropeValidations = map(
                ::fail1 = { return false },
                ::fail2 = { return 'bad' }
            )

            expect(false, #test->ropeIsValid(`fail1`))
            expect(false, #test->ropeIsValid(`fail2`))
        }

        it(`returns true if the tag-specified validation returns true or void`) => {
            local(test) = test_typeValidator
            #test->ropeValidations = map(
                ::pass1 = { return true },
                ::pass2 = { return      }
            )

            expect(true, #test->ropeIsValid(`pass1`))
            expect(true, #test->ropeIsValid(`pass2`))
        }
        it(`returns false if the tag-specfied validation returns string or false`) => {
            local(test) = test_typeValidator
            #test->ropeValidations = map(
                ::fail1 = { return false },
                ::fail2 = { return 'bad' }
            )
            
            expect(false, #test->ropeIsValid(`fail1`))
            expect(false, #test->ropeIsValid(`fail2`))
        }

        it(`returns true if all are valid`) => {
            local(test) = test_typeValidator
            #test->ropeValidations = map(
                ::fail1 = { return true },
                ::fail2 = { return true },
                ::pass1 = { return true }
            )

            expect(#test->ropeIsValid)
        }
        it(`returns false if any are invalid`) => {
            expect(not test_typeValidator->ropeIsValid)
        }
    }
}

describe(::rv_notEmpty) => {
    it(`returns a memberstream with method name of rv_notEmpty when called with no parameters`) => {
        local(t) = rv_notEmpty
        expect(::memberstream, #t->type)
    }

    context(`passed a value to check`) => {
        it(`returns the default validation message if the value is void`) => {
            expect("Field can not be empty", rv_notEmpty(void))
        }
        it(`returns the default validation message if the value is null`) => {
            expect("Field can not be empty", rv_notEmpty(null))
        }
        it(`returns the default validation message if value->size is zero`) => {
            expect("Field can not be empty", rv_notEmpty(``))
        }
        it(`returns void if none of the other conditions are met`) => {
            expect(void, rv_notEmpty('ha!'))
        }

        it(`allows the message to be set by an optional second param`) => {
            expect(`rhinos run rapidly, reaching rates of 35 mph`, rv_notEmpty(void, `rhinos run rapidly, reaching rates of 35 mph`))
        }
    }
}

describe(::rv_containedIn) => {
    it(`Takes in an object that understands the >> operator and returns a memberstream of the method to call`) => {
        expect(::memberstream, rv_containedIn((:1,'a'))->type)
    }
    it(`takes anobject that understands the >> operator and a string to use for message and returns a memberstream of the method to call`) => {
        expect(::memberstream, rv_containedIn((:1, 'a'), 'bad')->type)
    }

    context(`passed both a value and a container list`) => {
        it(`returns void if the value is contained by the specified set`) => {
            expect(void, rv_containedIn('a', (:1,'a')))
        }
        it(`returns the default validation message if the value is not in the set`) => {
            expect(`Field must be one of 1, a`, rv_containedIn('z', (:1,'a')))
        }
        it(`allows an optional custom validation message to be returned if the value is not in the set`) => {
            expect(`rhinos run rapidly, reaching rates of 35 mph`, rv_containedIn('z', (:1,'a'), `rhinos run rapidly, reaching rates of 35 mph`))
        }
    }

    context(`invoked with a value to check`) => {
        local(contains_test) = rv_containedIn((:'a', 'b', 'c'))
        it(`returns void if the value is contained by the specified set`) => {
            expect(void, #contains_test(`b`))
        }
        it(`returns the default validation message if the value is not in the set`) => {
            expect("Field must be one of a, b, c", #contains_test(`Z`))
        }
        it(`allows an optional custom validation message to be returned if the value is not in the set`) => {
            expect(`rhinos run rapidly, reaching rates of 35 mph`, #contains_test('z', `rhinos run rapidly, reaching rates of 35 mph`))
        }
    }
}

describe(::rv_inRange) => {
    it(`Takes in a generateSeries object and returns a memberstream of the method to call`) => {
        expect(::memberstream, rv_inRange(1 to 10)->type)
    }
    it(`takes a generateSeries and string to use for message and returns a memberstream of the method to call`) => {
        expect(::memberstream, rv_inRange(1 to 10, `bad`)->type)
    }

    context(`passed both a value and a generateSeries`) => {
        it(`returns void if the value is in the series`) => {
            expect(void, rv_inRange(3, 1 to 5))
        }
        it(`returns the default validation message if the value is not in the series`) => {
            expect(`Field must be between 1 and 5`, rv_inRange(-3, 1 to 5))
        }
        it(`allows an optional custom validation message to be returned if the value is not in the series`) => {
            expect(`rhinos run rapidly, reaching rates of 35 mph`, rv_inRange(-3, 1 to 5, `rhinos run rapidly, reaching rates of 35 mph`))
        }
    }

    context(`invoked with a value to check`) => {
        local(contains_test) = rv_inRange(1 to 5)
        it(`returns void if the value is contained by the specified set`) => {
            expect(void, #contains_test(3))
        }
        it(`returns the default validation message if the value is not in the set`) => {
            expect("Field must be between 1 and 5", #contains_test(-3))
        }
        it(`allows an optional custom validation message to be returned if the value is not in the set`) => {
            expect(`rhinos run rapidly, reaching rates of 35 mph`, #contains_test(-3, `rhinos run rapidly, reaching rates of 35 mph`))
        }
    }
}