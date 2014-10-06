if(not var_defined('_rope_loaded')) => {
    // This should let us run this anywhere and still properly import the file
    local(path_here) = currentCapture->callsite_file->stripLastComponent
    not #path_here->beginsWith('/')? #path_here = io_file_getcwd + '/' + #path_here
    not #path_here->endsWith('/') ? #path_here += '/'
    local(f) = file(#path_here + 'spec_helper.inc')
    sourcefile(#f->readString, #f->path, false, false)->invoke
}

describe(`rope -> link`) => {
    context(`controller has one simple path`) => {
        it(`returns the simple web-root path for a specified controller name`) => {
            rope->register('test1', -routes=(:'/foo/bar/baz/')) => {}

            expect(`/foo/bar/baz`, rope->link(`test1`))
        }
        it(`caches the link for faster lookups later`) => {
            rope->register('funk', -routes=(:'/rhino')) => {}
            expect('/rhino', rope->link('funk'))

            rope->deregister('funk')
            rope->register('funk', -routes=(:'/moose')) => {}
            expect('/rhino', rope->link('funk'))
        }
        it(`even works on the plain root path`) => {
            rope->register(`root`, -routes=(:`/`)) => {}

            expect(`/`, rope->link('root'))
        }
    }

    context(`controller has multiple simple paths`) => {
        beforeAll => {
            rope->register('simple-multi', -routes=(:'/foo/bar', '/moose/hair', '/rhino/run')) => {}
        }

        it(`returns the first path if no path index is specified`) => {
            expect('/foo/bar', rope->link('simple-multi'))
        }
        it(`returns the path at the specified path index`) => {
            expect('/foo/bar'   , rope->link('simple-multi', 1))
            expect('/moose/hair', rope->link('simple-multi', 2))
            expect('/rhino/run' , rope->link('simple-multi', 3))
        }
    }

    context(`controller has path with URL parameters`) => {
        beforeAll => {
            rope->register('url-params', -routes=(:'/rhino/:kind/run', '/user/:id/photo/:id')) => {}
        }
        it(`throws an error if it can't fill all the parameters`) => {
            expect->errorMsg('Link has missing dynamic values') => {
                rope->link('url-params')
            }
            expect->errorMsg('Link has missing dynamic values') => {
                rope->link('url-params', 2, (:33))
            }
        }
        it(`returns the path with the specified values filled in`) => {
            expect('/rhino/white/run', rope->link('url-params', (:'white')))
            expect('/user/42/photo/7', rope->link('url-params', 2, (:42, 7)))
        }
        it(`returns the path with the filled-in values URL encoded`) => {
            expect(`/rhino/white%2Fgrey/run`, rope->link('url-params', (:'white/grey')))
        }
    }
}