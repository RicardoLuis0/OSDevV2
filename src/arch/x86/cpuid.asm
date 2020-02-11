[BITS 32]
section .text
global has_cpuid
has_cpuid:
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
    pop ebx
    pop eax
    mov [eax],edx
    pop eax
    mov [eax],ecx
    push ebx
    ret
