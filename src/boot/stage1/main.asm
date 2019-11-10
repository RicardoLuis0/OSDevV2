;stage1/main
;http://vitaly_filatov.tripod.com/ng/asm/
[BITS 16]       ; 16 bits hint
;stage 1,512b stack,stage 2
section .asm_stage1
asm_stage1_start:
    ;setup
    mov [boot_drive], dl
                ; store boot drive number
    xor ax, ax  ; clear ax register
    mov ds, ax  ; set data segment register to 0
    mov ax, stack_end
    mov ss, ax  ; setup stack
    mov sp, stack_end+512
    cld         ; clear direction flag for string
    push msg1
    call asm_print
    call load_stage2
    push msg2
    call asm_print
    jmp asm_stage2_start

hang:
    jmp hang    ;hang system

%include "src/boot/stage1/print.asm"
%include "src/boot/stage1/hdd.asm"

boot_drive:
    db 0
msg1:
    db "Loading Second Stage...",13,10,0
msg2:
    db "Calling Second Stage...",13,10,0
    times 510-($-$$) db 0;use extra space as stack
                ;empty space at end of boot sector
    db 0x55
    db 0xAA     ; magic number
stack_end:

%include "src/boot/stage2/main.asm"
