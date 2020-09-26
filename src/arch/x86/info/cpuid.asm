[BITS 32]
section .text
global has_cpuid_impl
has_cpuid_impl:
    pushfd
    pushfd
    xor dword [esp],0x00200000
    popfd
    pushfd
    pop eax
    xor eax,[esp]
    popfd
    and eax,0x00200000
    ret

global cpuid_features
cpuid_features:
    mov eax,1
    mov ecx,0
    mov edx,0
    cpuid
    pop ebx ; save return address
    pop eax ; pop pointer into eax
    mov [eax],edx ; dereference and save edx into pointer
    pop eax ; pop pointer into eax
    mov [eax],ecx ; dereference and save ecx into pointer
    push ebx ; restore return address
    ret
