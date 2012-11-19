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
                expect(rope->controllerExist('new'))
                expect(rope->controller('new')->isA(::capture) > 0)
            }
            it(`registers any routes passed to the controller`) => {
                local(num_routes) = rope->routesSized(2)->size
                rope->register('new', -routes=(:'/moose/hair', '/foo/bar')) => {}
                expect(2 + #num_routes, rope->routesSized(2)->size)
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
                local(num_routes) = rope->routesSized(1)->size
                
                rope->register('exists', -routes=(:'hair', 'brush'))
                expect(2 + #num_routes, rope->routesSized(1)->size)
            }
        }
        
        it(`allows for registerd controllers / routes to be accessible in other threads`) => {
            local(_, readPipe) = split_thread => {
                local(writePipe) = #1->first
                handle => { #writePipe->writeObject(`Done`) }

                rope->register('otherThread') => {}
            }
            while(true) => { `Done` == #readPipe->readObject? loop_abort }
            
            expect(rope->controllerExist('otherThread'))
            expect(rope->controller('otherThread')->isA(::capture) > 0)
        }
    }
    
    describe(`-> matchRoute`) => {
        beforeAll => {
            rope->register('matchRoute') => {}
        }
        it(`throws an error if it can't find a matching route`) => {
            expect->error(error_code_invalidParameter, `No matching route found`) => {
                rope->matchRoute(`/this/path/no/exists`)
            }
        }
        it(`matches routes if the whole path matches`) => {
            rope->register('matchRoute', -routes=(:'/fly/you/fools'))
            expect('matchRoute', rope->matchRoute('/fly/you/fools')->second)
        }
        it(`throws an error if it matches more than one route`) => {
            rope->register('matchRoute', -routes=(:'/duplicate/route', '/duplicate/route'))
            
            expect->error(error_code_runtimeAssertion, `Multiple routes matched the given path`) => {
                rope->matchRoute('/duplicate/route')
            }
        }
        it(`allows for wildcard (:name) parts of the path to match anything`) => {
            rope->register('matchRoute', -routes=(:'/run/for/it/:name'))

            expect(`matchRoute`, rope->matchRoute('/run/for/it/Marty')->second)
        }
        it(`prefers matches where where the earlier parts are not wildcards`) => {
            rope->register('matchRoute' , -routes=(:'/great/:dane'))
            rope->register('matchRoute2', -routes=(:'/:adj/scott')) => {}
            
            expect(`matchRoute`, rope->matchRoute('/great/scott')->second)
        }
    }
    
    describe(`-> invoke`) => {
        it(`matches the URL to a route and invokes the route's controller`) => {
            web_request = mock('requestUri' = '/person/robinmckinley')
            rope->register(`invoke`, -routes=(:'/person/:id')) => { return 'sunshine' }
            
            expect(`sunshine`, rope->invoke)
        }
    }
}