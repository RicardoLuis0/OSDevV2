;boot.asm
[ORG 0x7c00]    ; hint that program is loaded at the 0x7c00 location in memory
    xor ax, ax  ; zero out ax register
    mov ds, ax  ; set code segment register
    mov si, hello_world
                ; move address of 'hello_world' message to the si register
    cld         ; clear direction flag for string
    call print  ;print 'hello_world' message

hang:
    jmp hang    ;hang system

print:
    lodsb       ; load byte from string at si into al
    or al, al   ; check for null terminator (if al is 0)
    jz return   ; if finished printing, return
    mov ah, 0x0E; set video interruptor function to teletype
    xor bh, bh  ; clear bh
    int 0x10    ; call video interruptor teletype function
    jmp print   ; loop
return:
    ret         ; return from procedure

hello_world:    ;'hello world' followed by 'CRLF' and null terminator
    db 'Hello World', 13, 10, 0
                ;empty space at end of boot sector
    times 510-($-$$) db 0
    db 0x55
    db 0xAA
