[BITS 32]
section .text

global ASM_paging_enable
ASM_paging_enable:;PAGE DIRECTORY MUST BE IN EAX, THIS IS A PURE ASSEMBLY FUNCTION, NOT A CDECL FUNCTION
    mov cr3, eax
    mov eax, cr0
    or eax, 0x80000001
    mov cr0, eax
    ret
