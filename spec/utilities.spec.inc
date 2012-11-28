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