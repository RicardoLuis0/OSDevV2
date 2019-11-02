;boot.asm
[ORG 0x7c00]    ; hint that program is loaded at the 0x7c00 location in memory
[BITS 16]       ; 16 bits hint
    xor ax, ax  ; zero out ax register
    mov ds, ax  ; set code segment register
    mov al, 0   ; scroll clear screen
    call scroll_up
    mov cx, 0xc ; move string length to cx
    mov dh, 0x0 ; y:0
    mov dl, 0x0 ; x:0
    mov bp, hello_world
                ; move address of 'hello_world' message to the si register
    cld         ; clear direction flag for string
    call print_strnxy
                ; print 'hello_world' message

hang:
    jmp hang    ;hang system

scroll_up:      ; scroll screen up by 'al' number of lines
    mov ah, 0x6 ; scroll up function ( vitaly_filatov.tripod.com/ng/asm/asm_023.7.html )
                ; al is provided by caller
    mov bh, 0x0 ; page 0
    mov ch, 0   ; start y:0
    mov cl, 0   ; start x:0
    mov dh, 0x18; end y:24
    mov dl, 0x4f; end x:79
    int 0x10     ; call video interruptor function
    ret          ; return from procedure

print_chartty:  ; print char at 'al' to screen at cursor
    mov ah, 0x0E; set video interruptor function to teletype
    xor bh, bh  ; clear bh
    int 0x10    ; call video interruptor teletype function
    ret         ; return

print_strtty:   ; print null terminated string at 'si' to screen at cursor
    lodsb       ; load byte from string at 'si' into 'al'
    or al, al   ; check for null terminator (if al is 0)
    jz return   ; if finished printing, return
    call print_chartty
                ; print char
    jmp print_strtty
                ; loop
return:
    ret         ; return from procedure

print_strnxy:    ; assumes 'cx' is string length, 'dh' is y, 'dl' is x, 'sb' and is string address
    mov ah, 0x13 ; write string function ( vitaly_filatov.tripod.com/ng/asm/asm_023.20.html )
    mov al, 0x0  ; color at 'bl'
    mov bh, 0x0  ; page 0
    mov bl, 0xF  ; white color
                 ; 'cx' is provided by caller
                 ; 'dh' is provided by caller
                 ; 'dl' is provided by caller
                 ; 'sb' is provided by caller 
    int 0x10     ; call video interruptor function
    ret          ; return from procedure

hello_world:    ;'hello world' followed by 'CRLF' and null terminator
    db 'Hello World', 13, 10, 0
                ;empty space at end of boot sector
    times 510-($-$$) db 0
    db 0x55
    db 0xAA
