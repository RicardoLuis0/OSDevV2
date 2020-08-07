[BITS 32]
section .rodata
unimp:db "CPU::enter_state unimplemented",0
unimp2:db "CPU::switch_state unimplemented",0

section .text

extern k_abort_s
;void enter_state(CpuState *);
global enter_state
enter_state:
    push unimp
    call k_abort_s
    ;TODO
    ret
    
;void switch_state(CpuState * from,CpuState * to);
global switch_state
switch_state:
    push unimp2
    call k_abort_s
    ;TODO
    ret


