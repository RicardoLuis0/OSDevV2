[BITS 32]
;https://wiki.osdev.org/Bare_Bones_with_NASM
MBALIGN  equ  1 << 0            ; align loaded modules on page boundaries
MEMINFO  equ  1 << 1            ; provide memory map
FLAGS    equ  MBALIGN | MEMINFO ; this is the Multiboot 'flag' field
MAGIC    equ  0x1BADB002        ; 'magic number' lets bootloader find the header
CHECKSUM equ -(MAGIC + FLAGS)   ; checksum of above, to prove we are multiboot

section .multiboot
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

section .bss ;stack
align 16
stack_bottom:
    resb 1048576;1MB stack
stack_top:

extern kmain
global start
global hang
section .text
start:;entrypoint
    mov esp, stack_top;setup the stack
    call kmain
    jmp hang

hang:
    cli
    hlt
    jmp hang
