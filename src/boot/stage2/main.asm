;stage2/main

section .asm_stage2
asm_stage2_start:
    push test
    call print
    jmp hang
test:
    db "It Works!",13,10,0
