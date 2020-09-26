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

global cpuid_0
cpuid_0:
    xor eax,eax
    xor ebx,ebx
    xor ecx,ecx
    xor edx,edx
    cpuid
    pop ebp ; save return address
    pop esi ; pop pointer into eax
    mov [esi],edx ; dereference and save edx into pointer
    pop esi ; pop pointer into eax
    mov [esi],ecx ; dereference and save ecx into pointer
    pop esi ; pop pointer into eax
    mov [esi],ebx ; dereference and save ecx into pointer
    pop esi ; pop pointer into eax
    mov [esi],eax ; dereference and save ecx into pointer
    push ebp ; restore return address
    ret

global cpuid_1
cpuid_1:
    mov eax,1
    xor ebx,ebx
    xor ecx,ecx
    xor edx,edx
    cpuid
    pop ebp ; save return address
    pop esi ; pop pointer into eax
    mov [esi],edx ; dereference and save edx into pointer
    pop esi ; pop pointer into eax
    mov [esi],ecx ; dereference and save ecx into pointer
    pop esi ; pop pointer into eax
    mov [esi],ebx ; dereference and save ecx into pointer
    pop esi ; pop pointer into eax
    mov [esi],eax ; dereference and save ecx into pointer
    push ebp ; restore return address
    ret

global cpuid_7_0
cpuid_7_0:
    mov eax,7
    xor ebx,ebx
    xor ecx,ecx
    xor edx,edx
    cpuid
    pop ebp ; save return address
    pop esi ; pop pointer into eax
    mov [esi],edx ; dereference and save edx into pointer
    pop esi ; pop pointer into eax
    mov [esi],ecx ; dereference and save ecx into pointer
    pop esi ; pop pointer into eax
    mov [esi],ebx ; dereference and save ecx into pointer
    pop esi ; pop pointer into eax
    mov [esi],eax ; dereference and save ecx into pointer
    push ebp ; restore return address
    ret

global cpuid_7_1
cpuid_7_1:
    mov eax,7
    xor ebx,ebx
    mov ecx,1
    xor edx,edx
    cpuid
    pop ebp ; save return address
    pop esi ; pop pointer into eax
    mov [esi],edx ; dereference and save edx into pointer
    pop esi ; pop pointer into eax
    mov [esi],ecx ; dereference and save ecx into pointer
    pop esi ; pop pointer into eax
    mov [esi],ebx ; dereference and save ecx into pointer
    pop esi ; pop pointer into eax
    mov [esi],eax ; dereference and save ecx into pointer
    push ebp ; restore return address
    ret
