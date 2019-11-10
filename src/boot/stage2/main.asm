;stage2/main
extern cmain
global print
section .asm_stage2
asm_stage2_start:
    push msg3
    call asm_print
    call cmain
    jmp hang
msg3:
    db "Calling CMain...",13,10,0
