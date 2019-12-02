;stage1/hdd

load_stage2_hdd_reset:
    mov ah, 0x0
    mov dl, [boot_drive]
    int 13h
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
    int 13h
    jc load_stage2
    ret
