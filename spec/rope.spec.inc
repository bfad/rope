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
            expect->error(error_code_fileNotFound, `No matching route found`) => {
                rope->matchRoute(`/this/path/no/exists`)
            }
        }
        it(`matches routes if the whole path matches`) => {
            rope->register('matchRoute', -routes=(:'/fly/you/fools'))
            expect('matchRoute', rope->matchRoute('/fly/you/fools')->second)
        }
        it(`can match the root route (/)`) => {
            rope->register(`matchRoute`, -routes=(:`/`))
            expect(`matchRoute`, rope->matchRoute('/')->second)
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
        beforeAll => {
            rope->register(`invoke`, -routes=(:'/person/:id')) => { return 'sunshine' }
            web_request = mock('requestUri' = '/person/robinmckinley')
        }
        it(`matches the URL to a route and invokes the route's controller`) => {
            expect(`sunshine`, rope->invoke)
        }
        
        it(`sets up the URL parameters`) => {
            expect(`robinmckinley`, rope->urlParam(`id`))
        }
    }
    
    
    describe(`-> include`) => {
        // Need to setup file_forceRoot so paths will be relative
        // However, can cause problems with lspec, especially when an expect fails
        // Therefore need to set $_file_forceroot_ = void at the end of every it (in a handle works).
        beforeAll => {
            web_response = web_response_mock('rawContent'='')
            var(my_root) = dir('//tmp/rope-' + lasso_uniqueID)->create&path
            dir($my_root + '/webroot/')->create
            dir($my_root + '/views/')->create
        }
        beforeEach => {file_forceRoot = $my_root + '/webroot/'}
        afterAll => {
            dir($my_root + '/webroot/')->delete
            dir($my_root + '/views/')->delete
            dir($my_root)->delete
            $_file_forceroot_ = void
        }
        it(`accepts a file object and throws an error if the file doesn't exist`) => {
            handle => { $_file_forceroot_ = void }
            
            local(f) = file(file_forceRoot(`/gone.lasso`))
            expect->error(error_code_resNotFound, `File Not Found: ` + #f->path) => {
                rope->include(#f)
            }
        }
        it(`accepts a file path and throws an error if the file does not exist`) => {
            handle => { $_file_forceroot_ = void }
            
            expect->error(error_code_resNotFound, `File Not Found: ` + file_forceRoot('/../views//gone.lasso')) => {
                rope->include(`/gone.lasso`)
            }
        }
        it(`returns the value of processing the specified file`) => {
            handle => { $_file_forceroot_ = void }
            
            local(template) = file(file_forceRoot('/basic.lasso'))
            handle => { #template->exists? #template->close&delete }
            
            #template->doWithClose => { #1->openWrite&writeBytes(bytes(`<?= 'Ho! ' * 3 ?>`)) }
            
            expect(`Ho! Ho! Ho! `, rope->include(#template))
        }
        it(`sets content_body to the value of processing the file at the specified path relative to webroot (../views/)`) => {
            handle => { $_file_forceroot_ = void }
            
            local(template) = file($my_root + '/views/basic2.lasso')
            handle => { #template->exists? #template->close&delete }
            #template->doWithClose => { #1->openWrite&writeBytes(bytes(`<?= local(noun) = {return 'roads'}->detach;#noun()->asCopy->titlecase& + "? Where we're going we don't need " + #noun() + "."; ?>`)) }
            
            expect(`Roads? Where we're going we don't need roads.`, rope->include('/basic2.lasso'))
        }
        it(`accepts named parameters that are setup as locals for the file being included`) => {
            handle => { $_file_forceroot_ = void }
            
            local(template) = file($my_root + '/views/locals.lasso')
            handle => { #template->exists? #template->close&delete }
            #template->doWithClose => { #1->openWrite&writeBytes(bytes(`<?= 'Hello, ' + #name + '.' ?>`)) }
            
            expect(`Hello, World.`, rope->include('locals.lasso', -name='World', -time=date))
        }
    }
    
    
    describe(`-> render`) => {
        // Need to setup file_forceRoot so paths will be relative
        // However, can cause problems with lspec, especially when an expect fails
        // Therefore need to set $_file_forceroot_ = void at the end of every it (in a handle works).
        beforeAll => {
            web_response = web_response_mock('rawContent'='')
            var(my_root) = dir('//tmp/rope-' + lasso_uniqueID)->create&path
            dir($my_root + '/webroot/')->create
            dir($my_root + '/views/')->create
        }
        beforeEach => {file_forceRoot = $my_root + '/webroot/'}
        afterAll => {
            dir($my_root + '/webroot/')->delete
            dir($my_root + '/views/')->delete
            dir($my_root)->delete
            $_file_forceroot_ = void
        }
        it(`accepts a file object and throws an error if the file doesn't exist`) => {
            handle => { $_file_forceroot_ = void }
            
            local(f) = file(file_forceRoot(`/gone.lasso`))
            expect->error(error_code_resNotFound, `File Not Found: ` + #f->path) => {
                rope->render(#f)
            }
        }
        it(`accepts a file path and throws an error if the file does not exist`) => {
            handle => { $_file_forceroot_ = void }
            
            expect->error(error_code_resNotFound, `File Not Found: ` + file_forceRoot('/../views//gone.lasso')) => {
                rope->render(`/gone.lasso`)
            }
        }
        it(`sets content_body to the value of processing the specified file`) => {
            handle => { $_file_forceroot_ = void }
            
            local(template) = file(file_forceRoot('/basic.lasso'))
            handle => { #template->exists? #template->close&delete }
            
            #template->doWithClose => { #1->openWrite&writeBytes(bytes(`<?= 'Ho! ' * 3 ?>`)) }
            rope->render(#template)
            expect(`Ho! Ho! Ho! `, content_body)
        }
        it(`sets content_body to the value of processing the file at the specified path relative to webroot (../views/)`) => {
            handle => { $_file_forceroot_ = void }
            
            local(template) = file($my_root + '/views/basic2.lasso')
            handle => { #template->exists? #template->close&delete }
            #template->doWithClose => { #1->openWrite&writeBytes(bytes(`<?= local(noun) = {return 'roads'}->detach;#noun()->asCopy->titlecase& + "? Where we're going we don't need " + #noun() + "."; ?>`)) }
            
            rope->render('/basic2.lasso')
            expect(`Roads? Where we're going we don't need roads.`, content_body)
        }
        it(`accepts named parameters that are setup as locals for the file being rendered`) => {
            handle => { $_file_forceroot_ = void }
            
            local(template) = file($my_root + '/views/locals.lasso')
            handle => { #template->exists? #template->close&delete }
            #template->doWithClose => { #1->openWrite&writeBytes(bytes(`<?= 'Hello, ' + #name + '.' ?>`)) }
            
            rope->render('locals.lasso', -name='World', -time=date)
            expect(`Hello, World.`, content_body)
        }
    }
    
    
    describe(`-> extension`) => {
        it(`returns the extention at the end of the URL path when it exits`) => {
            web_request = mock('requestUri' = '/long.ish/path.with/multiple.ext.init')
            
            expect(`init`, rope->extension)
        }
        it(`returns html if there is no extention at the end of the path`) => {
            web_request = mock('requestUri' = '/long.ish/path.with/multiple.ext/init')
            
            expect(`html`, rope->extension)
        }
    }
    
    
    describe(`-> renderJSON`) => {
        beforeAll => { web_response = web_response_mock('rawContent'='') }
        it(`sets the Content-type header for json`) => {
            rope->renderJSON('')
            expect('application/json; charset=UTF-8', web_response->header(`Content-type`))
        }
        it(`takes a string it assumes is json_serialize and sets it to content_body`) => {
            rope->renderJSON('moosehair')
            expect('moosehair', content_body)
        }
        it(`takes an array and sets the content_body to a serialized form of the array`) => {
            rope->renderJSON(array(1,2,3))
            expect('[1, 2, 3]', content_body)
        }
        it(`takes a staticarray and sets the content_body to a serialized form of the array`) => {
            rope->renderJSON((:'1','2','3'))
            expect('["1", "2", "3"]', content_body)
        }
        it(`takes a map and sets the content_body to a serialize form of that map`) => {
            rope->renderJSON(map('moose'='hair'))
            expect('{"moose": "hair"}', content_body)
        }
    }
}