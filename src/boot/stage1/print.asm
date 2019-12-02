;stage1/print
global asm_print
asm_print:  ;string to print is passed as stack parameter
    pop ax  ;pop return address
    pop si  ;pop parameter
    push ax ;preserve return address
    call asm_print_strtty
    ret

global asm_print_chartty
asm_print_chartty:  ; print char at 'al' to screen at cursor
    mov ah, 0x0E; set video interruptor function to teletype
    xor bh, bh  ; clear bh
    int 10h    ; call video interruptor teletype function
    ret         ; return

global asm_print_strtty
asm_print_strtty:   ; print null terminated string at 'si' to screen at cursor
    lodsb       ; load byte from string at 'si' into 'al'
    or al, al   ; check for null terminator (if al is 0)
    jz print_strtty_return
                ; if finished printing, return
    call asm_print_chartty
                ; print char
    jmp asm_print_strtty
                ; loop
print_strtty_return:
    ret         ; return from procedure
