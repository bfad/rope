if(not var_defined('_rope_loaded')) => {
    // This should let us run this anywhere and still properly import the file
    local(path_here) = currentCapture->callsite_file->stripLastComponent
    not #path_here->beginsWith('/')? #path_here = io_file_getcwd + '/' + #path_here
    not #path_here->endsWith('/') ? #path_here += '/'
    local(f) = file(#path_here + 'spec_helper.inc')
    sourcefile(#f->readString, #f->path, false, false)->invoke
}

describe(::rope) => {
    describe(`-> register`) => {
        context(`passed a givenBlock`) => {
            it(`registers the givenBlock to the controller`) => {
                rope->register('new') => {}
                expect(rope->controllers->contains('new'))
                expect(rope->controllers->get('new')->isA(::capture) > 0)
            }
            it(`registers any routes passed to the controller`) => {
                local(num_routes) = rope->routes->find(2)->size
                rope->register('new', -routes=(:'/moose/hair', '/foo/bar')) => {}
                expect(2 + #num_routes, rope->routes->find(2)->size)
            }
        }
        context(`no givenBlock passed`) => {
            beforeAll => {
                rope->register('exists') => {}
            }
            it(`throws an error if not passed any routes to assign to the controller`) => {
                expect->error(error_code_runtimeAssertion, `This method requires you to either also pass routes or a givenBlock`) => {
                    rope->register('exists')
                }
            }
            it(`throws an error if the specified controller does not exist`) => {
                expect->error(error_code_invalidParameter, `The specified controller does not exist`) => {
                    rope->register('noexist', -routes=(:'/moose'))
                }
            }
            it(`fails if one of the route params is not a string`) => {
                expect->error(error_code_invalidParameter, `Invalid parameter: One of the routes passed was not a string`) => {
                    rope->register('exists', -routes=(:'bar', 3))
                }
            }
            it(`registers the specified routes to the controller`) => {
                local(num_routes) = rope->routes->find(1)->size
                
                rope->register('exists', -routes=(:'hair', 'brush'))
                expect(2 + #num_routes, rope->routes->find(1)->size)
            }
        }
    }
}