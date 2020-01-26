[BITS 32]
;adapted from https://wiki.osdev.org/GDT_Tutorial, accessed on decembeer 5 2019
section .text
gdtr DW 0 ;limit
     DD 0 ;base

global loadgdt
loadgdt:
    mov   eax, [esp + 4]
    mov   [gdtr + 2], eax
    mov   ax, [esp + 8]
    mov   [gdtr], ax
    lgdt  [gdtr]
    ; reload cs register containing code selector:
    jmp   0x08:reload_cs ; 0x08 points at the new code selector
reload_cs:
    ; reload data segment registers:
    mov   ax, 0x10 ; 0x10 points at the new data selector
    mov   ds, ax
    mov   es, ax
    mov   fs, ax
    mov   gs, ax
    mov   ss, ax
    ret
