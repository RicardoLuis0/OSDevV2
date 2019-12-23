[BITS 32]
section .text
extern call_idt
%macro IDTX 1
global _idtx%1
_idtx%1:
    pusha ;save registers
    push %1 ;push irq number to stack
    call call_idt ;call c++ irq handler
    add esp,4 ;decrement stack
    popa ;restore registers
    iret
%endmacro

%assign i 0
%rep 256
IDTX i
%assign i i+1
%endrep
