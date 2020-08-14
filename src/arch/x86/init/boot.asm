[BITS 32]
;https://wiki.osdev.org/Bare_Bones_with_NASM
    MBALIGN     equ  1 << 0            ; align loaded modules on page boundaries
    MEMINFO     equ  1 << 1            ; provide memory map
    VIDINFO     equ  1 << 2            ; provide video mode info
    FLAGS       equ  MBALIGN | MEMINFO ; | VIDINFO
                                       ; this is the Multiboot 'flag' field
    MAGIC       equ  0x1BADB002        ; 'magic number' lets bootloader find the header
    CHECKSUM    equ  -(MAGIC + FLAGS)  ; checksum of above, to prove we are multiboot
;    STACK_SIZE  equ  32768            ; 32K stack
   STACK_SIZE  equ  1048576            ; 1MB stack
%define ARCH x86

section .multiboot
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

section .bss ;stack
align 16
stack_bottom:
    resb STACK_SIZE
stack_top:

extern %[ARCH]_start
extern k_main
extern _fini
global _start
global hang
section .text
_start:;entrypoint
    mov esp, stack_top;setup the stack
    push eax
    push ebx
    cli
    call %[ARCH]_start
    add esp,8
    call k_main
    call _fini
    jmp hang

hang:
    cli
    hlt
    jmp hang
