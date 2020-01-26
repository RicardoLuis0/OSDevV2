[BITS 32]
section .text
extern call_idt
extern hang

idtr DW 0 ;limit
     DD 0 ;base

global loadidt
loadidt:
    mov   eax, [esp + 4]
    mov   [idtr + 2], eax
    mov   ax, [esp + 8]
    mov   [idtr], ax
    lidt  [idtr]
    sti
    ret

%macro IDTES 1 ;exception handler irq no parameter
global _idtx%1
_idtx%1:
    pushad;save registers
    push 0
    push %1 ;push irq number to stack
    call call_idt ;call c++ irq handler
    add esp,8;pop 2 bytes (function arguments)
    popad;restore registers
    iret
%endmacro

%macro IDTEP 1 ;exception handler irq with parameter
global _idtx%1
_idtx%1:
    pushad ;save registers
    mov eax,[esp+0x20]
    push eax
    push %1 ;push irq number to stack
    call call_idt ;call c++ irq handler
    add esp,8;pop 2 bytes (function arguments)
    popad ;restore registers
    add esp,4;pop 1 byte (parameter)
    iret
%endmacro

%macro IDTX 1 ;general-purpose interrupt, takes parameter in eax
global _idtx%1
_idtx%1:
    pushad ;save registers
    push eax
    push %1 ;push irq number to stack
    call call_idt ;call c++ irq handler
    add esp,8;pop 2 bytes (function arguments)
    popad ;restore registers
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
