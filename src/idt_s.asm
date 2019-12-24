[BITS 32]
section .text
extern call_idt
extern hang

%macro IDTES 1 ;all general-purpose interrupts take an 'int' parameter on the stack, and pops it
global _idtx%1
_idtx%1:
    pushad ;save registers
    push 0
    push %1 ;push irq number to stack
    call call_idt ;call c++ irq handler
    add esp,8 ;decrement stack
    popad ;restore registers
    iret
%endmacro

%macro IDTEP 1 ;all general-purpose interrupts take an 'int' parameter on the stack, and pops it
global _idtx%1
_idtx%1:
    pushad ;save registers
    mov eax,[esp-32]
    push eax
    push %1 ;push irq number to stack
    call call_idt ;call c++ irq handler
    add esp,8 ;decrement stack
    popad ;restore registers
    add esp,4
    iret
%endmacro

%macro IDTX 1 ;all general-purpose interrupts take an 'int' parameter on the stack, and pops it
global _idtx%1
_idtx%1:
    pushad ;save registers
    mov eax,[esp-36]
    push eax
    mov eax,[esp-32]
    mov [esp-40],eax
    push %1 ;push irq number to stack
    call call_idt ;call c++ irq handler
    add esp,8 ;decrement stack
    popad ;restore registers
    add esp,4
    iret
%endmacro

%assign i 0

%rep 8
IDTES i ;0-7 no error code
%assign i i+1
%endrep

IDTEP i ;8 error code
%assign i i+1

IDTES i ;9 no error code
%assign i i+1

%rep 5
IDTEP i ;10-14 error code
%assign i i+1
%endrep

%rep 2
IDTES i ;15-16 no error code, 15 reserved
%assign i i+1
%endrep

IDTEP i ;17 error code
%assign i i+1

%rep 12
IDTES i ;18-29 no error code, 21-29 reserved
%assign i i+1
%endrep

IDTEP i ;30 error code
%assign i i+1

IDTES i ;31 no error code
%assign i i+1

%rep 224
IDTX i
%assign i i+1
%endrep
