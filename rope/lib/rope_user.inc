define trait_rope_user => trait {
    import trait_serializable
    
    require isAuthenticated
}

define rope_user => type {
    trait { import trait_rope_user }
    
    data private id::integer,
         private nameFirst::string,
         private nameLast ::string,
         private username ::string
    // Public Getters
    public 
        id        => .`id`       ,
        nameFirst => .`nameFirst`,
        nameLast  => .`nameLast` ,
        username  => .`username`
    
    public onCreate() => {}
    public onCreate(id::integer, name_first::string, name_last::string, username::string) => {
        .`id`        = #id
        .`nameFirst` = #name_first
        .`nameLast`  = #name_last
        .`username`  = #username
    }
    public onCreate(id::integer) => .onCreate(#id, '', '', '')
    public onCreate(id::integer, username::string) => .onCreate(#id, '', '', #username)
    public onCreate(id::integer, name_first::string, name_last::string) => .onCreate(#id, #name_first, #name_last, '')
    
    public isAuthenticated => (.id->isA(::integer) != 0)
    
    
    public acceptDeserializedElement(d::serialization_element) => {
        self->\(tag(#d->key + '='))->invoke(#d->value)
	}

	public serializationElements() => {
		return array(
		    serialization_element('id', .'id'),
		    serialization_element('username' , .'username'),
		    serialization_element('nameFirst', .'nameFirst'),
		    serialization_element('nameLast' , .'nameLast')
		)
	}
}