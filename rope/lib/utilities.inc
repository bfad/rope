define  string->encodeUrl(strict::boolean=true) => bytes(self)->encodeUrl(#strict)
define integer->encodeUrl(strict::boolean=true) => bytes(self->asString)

define rope_cycle(...) => { 
    return { 
        local(i) = #rest->size
        while(#i--) => { yield #rest->get(loop_count) } 
        currentCapture->restart
    }->detach 
}