;stage2/main
extern cmain
section .asm_stage2
asm_stage2_start:
    push msg3
    call asm_print
    call cmain
    jmp hang
msg3:
    db "Calling cmain...",13,10,0

global asm_move_xy
asm_move_xy:;move cursor
    pop bx
    pop ax
    mov dl,al;column
    pop ax
    mov dh,al;row
    push bx
    xor bx,bx
    mov ah,02
    int 10h
    ret

global asm_cls
asm_cls:
    mov ah,6h
    mov al,0
    mov bh,0
    mov ch,0
    mov cl,0
    mov dh,25
    mov dl,80
    int 10h
;    mov ah,02
;    mov dl,0
;    mov dh,0
;    mov dl,0
;    int 10h
    ret
