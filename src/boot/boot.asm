;boot.asm
    cli
hang:
    jmp hang;loop
    times 510-($-$$) db 0
    db 0x55
    db 0xAA
