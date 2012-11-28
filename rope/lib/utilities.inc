define  string->encodeUrl(strict::boolean=true) => bytes(self)->encodeUrl(#strict)
define integer->encodeUrl(strict::boolean=true) => bytes(self->asString)

define rope_cycle(...) => { 
    return { 
        local(i) = #rest->size
        while(#i--) => { yield #rest->get(loop_count) } 
        currentCapture->restart
    }->detach 
}

define rope_hash(plain::string, salt::string, cost::integer=1) => {
    local(cyphertext) = #plain
    loop(#cost) => { #cyphertext = cipher_digest(#salt + #cyphertext, -digest='RIPEMD160') }
    return #cyphertext->encodeHex
}

define rope_randomASCII(len::integer) => {
    local(ret) = ''
    while(#len--) => {
        #ret->append(bytes->import8bits(math_random(32,126))&asString)
    }
    return #ret
}