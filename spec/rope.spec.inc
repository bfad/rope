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
            it(`registers the file path where the given block is registered`) => {
                rope->register(`new`) => {}
                expect(rope->absPath(currentCapture->callsite_file), rope->controllerPath('new'))
            }
            it(`overwrites an existing controller with the same name`) => {
                rope->register(`new-rewrite`) => { return 'first' }
                local(first) = rope->controller(`new-rewrite`)->invoke
                
                rope->register(`new-rewrite`) => { return 'second' }
                local(second) = rope->controller(`new-rewrite`)->invoke
                
                expect('first' , #first)
                expect('second', #second)
            }
            it(`ignores registering the same route to the same controller`) => {
                local(num_routes) = rope->routesSized(2)->size
                rope->register('new', -routes=(:'/dupe/route', '/dupe/route')) => {}
                
                expect(1 + #num_routes, rope->routesSized(2)->size)
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
    
    describe(`-> deregister`) => {
        it(`removes the specified controller from the system`) => {
            rope->register('deregister') => {}
            expect(rope->controllerExist('deregister'))
            
            rope->deregister(`deregister`, -routes=(:`/deregister`))
            expect(not rope->controllerExist('deregister'))
            expect(void, rope_thread->code_paths->find('deregister'))
            expect->error(error_code_fileNotFound, `No matching route found`) => {
                rope->matchRoute(`/deregister`)
            }
        }
    }
    describe(`-> deregisterAll`) => {
        it(`removes all the registered controllers`) => {
            local(num_ctrls) = rope_thread->controllers->size
            rope->register('deregisterAll') => {}
            expect(1 + #num_ctrls, rope_thread->controllers->size)
            
            rope->deregisterAll
            expect(0, rope_thread->controllers->size)
            expect(0, rope_thread->routes->size)
            expect(0, rope_thread->code_paths->size)
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
            rope->register('matchRoute', -routes=(:'/duplicate/route'))
            rope->register('matchRote' , -routes=(:'/duplicate/route')) => {}
            
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
        
        it(`if rope-fallback controller is defined it is called when no route matches`) => {
            expect->error(error_code_fileNotFound, `No matching route found`) => {
                rope->matchRoute(`/this/path/no/exists`)
            }
            rope->register(`rope-fallback`) => {}
            
            expect(`rope-fallback`, rope->matchRoute(`/this/path/no/exists`)->second)
        }
    }
    
    describe(`-> invoke`) => {
        beforeAll => {
            rope->register(`invoke`, -routes=(:'/person/:id')) => { return 'sunshine' }
            new_fake_conn
            var(_req_params) = map(::REQUEST_URI='/person/robinmckinley')
        }
        it(`matches the URL to a route and invokes the route's controller`) => {
            expect(`sunshine`, rope->invoke)
        }
        
        it(`sets up the URL parameters`) => {
            expect(`robinmckinley`, rope->urlParam(`id`))
        }
        
        it(`loads the RopeSession`) => {
            protect => { session_end(`RopeSession`) }
            
            rope->invoke
            expect(session_id(`RopeSession`)->size > 0)
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
    
    
    describe(`-> reload`) => {
        it(`reprocesses the file where the specified controller was registered`) => {
            local(f) = file(`//tmp/rope-` + lasso_uniqueID + '.inc')
            handle => { #f->exists? #f->delete }
            
            #f->doWithClose => { #1->openWrite&writeBytes(bytes(`rope->register('reload') => { return 'moose' }`)) }
            import_file(#f)
            local(first) = rope->controller('reload')->invoke
            
            #f->doWithClose => { #1->openTruncate&writeBytes(bytes(`rope->register('reload') => { return 'hair' }`)) }
            rope->reload('reload')
            local(second) = rope->controller('reload')->invoke
            
            expect('moose', #first)
            expect('hair', #second)
        }
        it(`reloads all registered controllers`) => {
            // First clear any controllers registered in here or we'll have an infinite loop!
            rope->deregisterAll
            
            local(f1) = file(`//tmp/rope-` + lasso_uniqueID + '.inc')
            local(f2) = file(`//tmp/rope-` + lasso_uniqueID + '.inc')
            handle => {
                #f1->exists? #f1->delete
                #f2->exists? #f2->delete
            }
            
            #f1->doWithClose => { #1->openWrite&writeBytes(bytes(`rope->register('reload1') => { return 'moose' }`)) }
            #f2->doWithClose => { #1->openWrite&writeBytes(bytes(`rope->register('reload2') => { return 'duck' }`)) }
            import_file(#f1)
            import_file(#f2)
            local(first1) = rope->controller('reload1')->invoke
            local(first2) = rope->controller('reload2')->invoke
            
            #f1->doWithClose => { #1->openTruncate&writeBytes(bytes(`rope->register('reload1') => { return 'hair' }`)) }
            #f2->doWithClose => { #1->openTruncate&writeBytes(bytes(`rope->register('reload2') => { return 'goose' }`)) }
            rope->reload
            local(second1) = rope->controller('reload1')->invoke
            local(second2) = rope->controller('reload2')->invoke
            
            expect('moose', #first1)
            expect('duck' , #first2)
            expect('hair' , #second1)
            expect('goose', #second2)
        }
    }
    
    
    describe(`-> continueSession`) => {
        beforeEach => {
            // Completely clear out any session named RopeSession
            protect => { session_end(`RopeSession`) }
            new_fake_conn
        }
        it(`starts a session named RopeSession if it doesn't exist`) => {
            expect(void, session_id(`RopeSession`))
            rope->continueSession
            expect(session_id(`RopeSession`)->size > 0)
        }
        it(`loads an already existing session on a new connection when passed the right credentials`) => {
            expect(void, session_id(`RopeSession`))
            rope->continueSession
            
            local(sess_id) = session_id(`RopeSession`)
            new_fake_conn
            $_req_params->insert(::HTTP_COOKIE="_LassoSessionTracker_RopeSession=" + #sess_id)
            rope->continueSession

            expect(#sess_id, session_id(`RopeSession`))
        }
        it(`doesn't start a session if sessions are turned off`) => {
            rope_thread->configLoadedFalse
            rope->config(`sessions-enabled` = false)
            
            expect(void, session_id(`RopeSession`))
            rope->continueSession
            expect(void, session_id(`RopeSession`))
        }
        it(`sets the expired time based on configuration option`) => {
            session_setDefaultDriver('memory')
            rope_thread->configLoadedFalse
            rope->config(`sessions-enabled` = true, 'sessions-expires' = 60)
            
            expect(void, session_id(`RopeSession`))
            rope->continueSession
            expect(session_id(`RopeSession`)->size > 0)
            
            expect(60, 
                with row in memory_session_driver->sessionsDump 
                where #row->first == session_id('RopeSession') + '_RopeSession'
                sum #row->second->expireMinutes
            )
        }
    }
    describe(`-> sess`) => {
        beforeAll => {
            rope_thread->configLoadedFalse
            rope->config(`sessions-enabled` = true)
            // Completely clear out any session named RopeSession
            protect => { session_end(`RopeSession`) }
            new_fake_conn
            rope->continueSession
            $_req_params->insert(::HTTP_COOKIE="_LassoSessionTracker_RopeSession=" + session_id(`RopeSession`))
        }
        it(`allows you to store data in a session`) => {
            expect(void, $_sess_rope->find('first'))
            rope->sess('first') = 'class'
            expect('class', $_sess_rope->find('first'))
        }
        it(`throws an error when the value on the rhs is not serializeable`) => {
            define notSerializable => type {}
            expect->error => {
                rope->sess('error') = notSerializable
            }
        }
        it(`allows you to retrieve previously stored session data`) => {
            rope->sess(`snake`) = `anaconda`
            
            expect(`anaconda`, rope->sess(`snake`))
        }
        context(`sessions disabled`) => {
            beforeAll => {
                rope_thread->configLoadedFalse
                rope->config(`sessions-enabled` = false)
            }
            it(`throws an error when trying to set a session value`) => {
                expect->error(error_code_runtimeAssertion, `Sessions are disabled`) => {
                    rope->sess('not') = 'possible'
                }
            }
            it(`throws an error when trying to get a session value`) => {
                expect->error(error_code_runtimeAssertion, `Sessions are disabled`) => {
                    rope->sess('error')
                }
            }
        }
    }
    
    
    describe(`-> config`) => {
        beforeEach => {
            rope_thread->configLoadedFalse
        }
        it(`takes a string as a key to lookup a preference`) => {
            expect(`UnNamed`, rope->config(`app-name`))
        }
        it(`allows configuration setup once by taking in pairs - second time fails`) => {
            rope->config(`app-name`=`MooseHair`)
            
            expect(`MooseHair`, rope->config(`app-name`))
            expect->error => {
                rope->config(`app-name`=`TEST`)
            }
        }
        it(`throws an error if the key to one of the configuration items does not exist`) => {
            expect->error => {
                rope->config(`NO EXIST AT ALL`=`TEST`)
            }
        }
        it(`throws an error if you try and register a value of an improper type to a config option`) => {
            expect->error(error_code_invalidParameter, `The "app-name" config option requires a string`) => {
                rope->config(`app-name`=1)
            }
        }
    }
    
    
    describe(`-> user`) => {
        beforeAll => {
            rope_thread->configLoadedFalse
            rope->config(`sessions-enabled` = true)
            // Completely clear out any session named RopeSession
            protect => { session_end(`RopeSession`) }
            new_fake_conn
            rope->continueSession
            $_req_params->insert(::HTTP_COOKIE="_LassoSessionTracker_RopeSession=" + session_id(`RopeSession`))
        }
        it(`allows for storing a user object in a session`) => {
            expect(null, $_sess_user)
            rope->user = rope_user(1, 'Ditka')
            expect('Ditka', $_sess_user->username)
        }
        it(`throws an error if not passed a serializeable object`) => {
            define notSerializable => type {}
            expect->error => {
                rope->user = notSerializable
            }
        }
        it(`allows a stored user object to be retrieved`) => {
            rope->user = rope_user(1, 'this is heavy')
            expect('this is heavy', rope->user->username)
        }
        context(`sessions disabled`) => {
            beforeAll => {
                rope_thread->configLoadedFalse
                rope->config(`sessions-enabled` = false)
            }
            it(`throw an error when setting the user if sessions are turned off`) => {
                expect->error(error_code_runtimeAssertion, `Sessions are disabled`) => {
                    rope->user = rope_user(1, 'foo')
                }
            }
            it(`throw an error when getting the user if sessions are turned off`) => {
                expect->error(error_code_runtimeAssertion, `Sessions are disabled`) => {
                    rope->user
                }
            }
        }
    }
    
    
    describe(`-> method`) => {
        it(`returns the HTTP method as found in the web_request`) => {
            new_fake_conn
            var(_req_params) = map(::REQUEST_METHOD='PUT')

            expect(bytes(`PUT`), bytes(rope->method))
        }
        it(`returns the value of a query parameter named _method when it exists instead of the value in the web_request`) => {
            new_fake_conn
            var(_req_params) = map(
                ::REQUEST_METHOD = 'GET', 
                ::REQUEST_URI    = '/http/method?_method=PUT',
                ::QUERY_STRING   = bytes(`_method=PUT`)
            )
            
            expect(`GET`, web_request->requestMethod)
            expect(`PUT`, rope->method)
        }
        it(`returns the value of a post parameter named _method when it exists instead of the value in the web_request`) => {
            new_fake_conn
            var(_req_params) = map(
                ::REQUEST_METHOD = 'POST', 
                ::REQUEST_URI    = '/http/method',
                ::CONTENT_TYPE   = 'application/x-www-form-urlencoded'
            )
            var(_req_stdin) = bytes(`_method=PUT`)
            
            expect(`POST`, web_request->requestMethod)
            expect(`PUT` , rope->method)
        }
        it(`returns the value of a url parameter named _method when it exists instead of the value in the web_request`) => {
            new_fake_conn
            var(_req_params) = map(
                ::REQUEST_METHOD = 'GET', 
                ::REQUEST_URI    = '/http/method/PUT'
            )
            rope->register(`http-method`, -routes=(:`/http/method/:_method`)) => {}
            rope->invoke
            
            expect(`GET`, web_request->requestMethod)
            expect(`PUT`, rope->method)
        }
    }
}