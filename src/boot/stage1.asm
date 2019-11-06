;boot.asm
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
    call load_stage2
    jmp asm_stage2_start

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

print:  ;string to print is passed as stack parameter
    pop ax  ;pop return address
    pop si  ;pop parameter
    push ax ;preserve return address
    call print_strtty
    ret

print_chartty:  ; print char at 'al' to screen at cursor
    mov ah, 0x0E; set video interruptor function to teletype
    xor bh, bh  ; clear bh
    int 0x10    ; call video interruptor teletype function
    ret         ; return

print_strtty:   ; print null terminated string at 'si' to screen at cursor
    lodsb       ; load byte from string at 'si' into 'al'
    or al, al   ; check for null terminator (if al is 0)
    jz print_strtty_return
                ; if finished printing, return
    call print_chartty
                ; print char
    jmp print_strtty
                ; loop
print_strtty_return:
    ret         ; return from procedure

print_strnxy:    ; assumes 'cx' is string length, 'dh' is y, 'dl' is x, 'sb' and is string address
    mov ah, 0x13 ; write string function ( vitaly_filatov.tripod.com/ng/asm/asm_023.20.html )
    cld          ; clear direction flag for string
    mov al, 0x0  ; color at 'bl'
    mov bh, 0x0  ; page 0
    mov bl, 0xF  ; white color
                 ; 'cx' is provided by caller
                 ; 'dh' is provided by caller
                 ; 'dl' is provided by caller
                 ; 'sb' is provided by caller 
    int 0x10     ; call video interruptor function
    ret          ; return from procedure


load_stage2_hdd_reset:;first
    mov ah, 0x0
    mov dl, [boot_drive]
    int 0x13
    jc load_stage2_hdd_reset;can hang
    ret

load_stage2:
    call load_stage2_hdd_reset
    mov ax, 0x0
    mov es, ax
    mov ah, 0x2
    mov al, 0x5 ;load 5(?) sectors
    mov ch, 0x0
    mov cl, 0x2
    mov dh, 0x0
    mov dl, [boot_drive]
    mov bx, asm_stage2_start
    int 0x13
    jc load_stage2
    ret

boot_drive:
    db 0
    times 510-($-$$) db 0;use extra space as stack
                ;empty space at end of boot sector
    db 0x55
    db 0xAA
stack_end:

%include "src/boot/stage2.asm"
