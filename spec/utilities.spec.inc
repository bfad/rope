if(not var_defined('_rope_loaded')) => {
    // This should let us run this anywhere and still properly import the file
    local(path_here) = currentCapture->callsite_file->stripLastComponent
    not #path_here->beginsWith('/')? #path_here = io_file_getcwd + '/' + #path_here
    not #path_here->endsWith('/') ? #path_here += '/'
    local(f) = file(#path_here + 'spec_helper.inc')
    sourcefile(#f->readString, #f->path, false, false)->invoke
}

describe(::rope_cycle) => {
    it(`returns an anonymous capture`) => {
        expect(::capture, rope_cycle->type)
    }
    it(`takes in a value that is returned each time the capture is invoked`) => {
        local(cycle) = rope_cycle(`rhino`)
        expect(`rhino`, #cycle())
        expect(`rhino`, #cycle())
        expect(`rhino`, #cycle())
    }
    it(`takes in multiple values and cycles through returning them each time the capture is invoked`) => {
        local(cycle) = rope_cycle(`moose`, `hair`)
        expect(`moose`, #cycle())
        expect(`hair` , #cycle())
        expect(`moose`, #cycle())
        expect(`hair` , #cycle())
        expect(`moose`, #cycle())
    }
}