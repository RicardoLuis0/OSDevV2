[BITS 32]
section .rodata
unimp:db "CPU::enter_state unimplemented",0

section .text

extern k_abort_s
global enter_state
enter_state:
    push unimp
    call k_abort_s
    ;TODO
    ret
    
