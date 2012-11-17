if(not var_defined('_rope_loaded')) => {
    // This should let us run this anywhere and still properly import the file
    local(path_here) = currentCapture->callsite_file->stripLastComponent
    not #path_here->beginsWith('/')? #path_here = io_file_getcwd + '/' + #path_here
    not #path_here->endsWith('/') ? #path_here += '/'
    local(f) = file(#path_here + 'spec_helper.inc')
    sourcefile(#f->readString, #f->path, false, false)->invoke
}

describe(::rope) => {
    describe(`-> route`) => {
        it(`requires a givenBlock to be passed to the method`) => {
            expect->error(error_code_runtimeAssertion, `The route member method requires a givenBlock`) => {
                rope->route('moose')
            }
        }
        it(`fails when called without any parameters`) => {
            expect->error(error_code_methodNotFound, `Definition Not Found: rope->route() Candidates were: rope->route(route::string, ...)`) => {
                rope->route() => {}
            }
        }
        it(`takes an undetermined amount of string paramaters`) => {
            local(params) = array
            loop(math_random(2,8)) => {
                #params->insert('ad' * loop_count)
            }
            rope->route(:#params) => {}
            expect(true)
        }
        it(`fails if one of the params is not a string`) => {
            expect->error(error_code_invalidParameter, `Invalid parameter: The route member method was passed an object that was not a string`) => {
                rope->route('foo', (:'bar'), 'baz') => {}
            }
        }
    }
}