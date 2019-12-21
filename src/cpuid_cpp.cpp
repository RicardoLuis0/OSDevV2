#include "cpuid.h"
#include "klib.h"
#include "screen.h"

enum cpuid_features_ecx{
    CPUID_FEAT_ECX_SSE3         = 1 << 0,
    CPUID_FEAT_ECX_PCLMUL       = 1 << 1,
    CPUID_FEAT_ECX_DTES64       = 1 << 2,
    CPUID_FEAT_ECX_MONITOR      = 1 << 3,
    CPUID_FEAT_ECX_DS_CPL       = 1 << 4,
    CPUID_FEAT_ECX_VMX          = 1 << 5,
    CPUID_FEAT_ECX_SMX          = 1 << 6,
    CPUID_FEAT_ECX_EST          = 1 << 7,
    CPUID_FEAT_ECX_TM2          = 1 << 8,
    CPUID_FEAT_ECX_SSSE3        = 1 << 9,
    CPUID_FEAT_ECX_CID          = 1 << 10,
    CPUID_FEAT_ECX_FMA          = 1 << 12,
    CPUID_FEAT_ECX_CX16         = 1 << 13,
    CPUID_FEAT_ECX_ETPRD        = 1 << 14,
    CPUID_FEAT_ECX_PDCM         = 1 << 15,
    CPUID_FEAT_ECX_PCIDE        = 1 << 17,
    CPUID_FEAT_ECX_DCA          = 1 << 18,
    CPUID_FEAT_ECX_SSE4_1       = 1 << 19,
    CPUID_FEAT_ECX_SSE4_2       = 1 << 20,
    CPUID_FEAT_ECX_x2APIC       = 1 << 21,
    CPUID_FEAT_ECX_MOVBE        = 1 << 22,
    CPUID_FEAT_ECX_POPCNT       = 1 << 23,
    CPUID_FEAT_ECX_AES          = 1 << 25,
    CPUID_FEAT_ECX_XSAVE        = 1 << 26,
    CPUID_FEAT_ECX_OSXSAVE      = 1 << 27,
    CPUID_FEAT_ECX_AVX          = 1 << 28,
};

enum cpuid_features_edx{
    CPUID_FEAT_EDX_FPU          = 1 << 0,
    CPUID_FEAT_EDX_VME          = 1 << 1,
    CPUID_FEAT_EDX_DE           = 1 << 2,
    CPUID_FEAT_EDX_PSE          = 1 << 3,
    CPUID_FEAT_EDX_TSC          = 1 << 4,
    CPUID_FEAT_EDX_MSR          = 1 << 5,
    CPUID_FEAT_EDX_PAE          = 1 << 6,
    CPUID_FEAT_EDX_MCE          = 1 << 7,
    CPUID_FEAT_EDX_CX8          = 1 << 8,
    CPUID_FEAT_EDX_APIC         = 1 << 9,
    CPUID_FEAT_EDX_SEP          = 1 << 11,
    CPUID_FEAT_EDX_MTRR         = 1 << 12,
    CPUID_FEAT_EDX_PGE          = 1 << 13,
    CPUID_FEAT_EDX_MCA          = 1 << 14,
    CPUID_FEAT_EDX_CMOV         = 1 << 15,
    CPUID_FEAT_EDX_PAT          = 1 << 16,
    CPUID_FEAT_EDX_PSE36        = 1 << 17,
    CPUID_FEAT_EDX_PSN          = 1 << 18,
    CPUID_FEAT_EDX_CLF          = 1 << 19,
    CPUID_FEAT_EDX_DTES         = 1 << 21,
    CPUID_FEAT_EDX_ACPI         = 1 << 22,
    CPUID_FEAT_EDX_MMX          = 1 << 23,
    CPUID_FEAT_EDX_FXSR         = 1 << 24,
    CPUID_FEAT_EDX_SSE          = 1 << 25,
    CPUID_FEAT_EDX_SSE2         = 1 << 26,
    CPUID_FEAT_EDX_SS           = 1 << 27,
    CPUID_FEAT_EDX_HTT          = 1 << 28,
    CPUID_FEAT_EDX_TM1          = 1 << 29,
    CPUID_FEAT_EDX_IA64         = 1 << 30,
    CPUID_FEAT_EDX_PBE          = 1 << 31,
};

void print_features(uint32_t ecx,uint32_t edx){
    if(ecx&CPUID_FEAT_ECX_SSE3){
        Screen::write_s("CPUID_FEAT_ECX_SSE3\n");
    }
    if(ecx&CPUID_FEAT_ECX_PCLMUL){
        Screen::write_s("CPUID_FEAT_ECX_PCLMUL\n");
    }
    if(ecx&CPUID_FEAT_ECX_DTES64){
        Screen::write_s("CPUID_FEAT_ECX_DTES64\n");
    }
    if(ecx&CPUID_FEAT_ECX_MONITOR){
        Screen::write_s("CPUID_FEAT_ECX_MONITOR\n");
    }
    if(ecx&CPUID_FEAT_ECX_DS_CPL){
        Screen::write_s("CPUID_FEAT_ECX_DS_CPL\n");
    }
    if(ecx&CPUID_FEAT_ECX_VMX){
        Screen::write_s("CPUID_FEAT_ECX_VMX\n");
    }
    if(ecx&CPUID_FEAT_ECX_SMX){
        Screen::write_s("CPUID_FEAT_ECX_SMX\n");
    }
    if(ecx&CPUID_FEAT_ECX_EST){
        Screen::write_s("CPUID_FEAT_ECX_EST\n");
    }
    if(ecx&CPUID_FEAT_ECX_TM2){
        Screen::write_s("CPUID_FEAT_ECX_TM2\n");
    }
    if(ecx&CPUID_FEAT_ECX_SSSE3){
        Screen::write_s("CPUID_FEAT_ECX_SSSE3\n");
    }
    if(ecx&CPUID_FEAT_ECX_CID){
        Screen::write_s("CPUID_FEAT_ECX_CID\n");
    }
    if(ecx&CPUID_FEAT_ECX_FMA){
        Screen::write_s("CPUID_FEAT_ECX_FMA\n");
    }
    if(ecx&CPUID_FEAT_ECX_CX16){
        Screen::write_s("CPUID_FEAT_ECX_CX16\n");
    }
    if(ecx&CPUID_FEAT_ECX_ETPRD){
        Screen::write_s("CPUID_FEAT_ECX_ETPRD\n");
    }
    if(ecx&CPUID_FEAT_ECX_PDCM){
        Screen::write_s("CPUID_FEAT_ECX_PDCM\n");
    }
    if(ecx&CPUID_FEAT_ECX_PCIDE){
        Screen::write_s("CPUID_FEAT_ECX_PCIDE\n");
    }
    if(ecx&CPUID_FEAT_ECX_DCA){
        Screen::write_s("CPUID_FEAT_ECX_DCA\n");
    }
    if(ecx&CPUID_FEAT_ECX_SSE4_1){
        Screen::write_s("CPUID_FEAT_ECX_SSE4_1\n");
    }
    if(ecx&CPUID_FEAT_ECX_SSE4_2){
        Screen::write_s("CPUID_FEAT_ECX_SSE4_2\n");
    }
    if(ecx&CPUID_FEAT_ECX_x2APIC){
        Screen::write_s("CPUID_FEAT_ECX_x2APIC\n");
    }
    if(ecx&CPUID_FEAT_ECX_MOVBE){
        Screen::write_s("CPUID_FEAT_ECX_MOVBE\n");
    }
    if(ecx&CPUID_FEAT_ECX_POPCNT){
        Screen::write_s("CPUID_FEAT_ECX_POPCNT\n");
    }
    if(ecx&CPUID_FEAT_ECX_AES){
        Screen::write_s("CPUID_FEAT_ECX_AES\n");
    }
    if(ecx&CPUID_FEAT_ECX_XSAVE){
        Screen::write_s("CPUID_FEAT_ECX_XSAVE\n");
    }
    if(ecx&CPUID_FEAT_ECX_OSXSAVE){
        Screen::write_s("CPUID_FEAT_ECX_OSXSAVE\n");
    }
    if(ecx&CPUID_FEAT_ECX_AVX){
        Screen::write_s("CPUID_FEAT_ECX_AVX\n");
    }
    if(edx&CPUID_FEAT_EDX_FPU){
        Screen::write_s("CPUID_FEAT_EDX_FPU\n");
    }
    if(edx&CPUID_FEAT_EDX_VME){
        Screen::write_s("CPUID_FEAT_EDX_VME\n");
    }
    if(edx&CPUID_FEAT_EDX_DE){
        Screen::write_s("CPUID_FEAT_EDX_DE\n");
    }
    if(edx&CPUID_FEAT_EDX_PSE){
        Screen::write_s("CPUID_FEAT_EDX_PSE\n");
    }
    if(edx&CPUID_FEAT_EDX_TSC){
        Screen::write_s("CPUID_FEAT_EDX_TSC\n");
    }
    if(edx&CPUID_FEAT_EDX_MSR){
        Screen::write_s("CPUID_FEAT_EDX_MSR\n");
    }
    if(edx&CPUID_FEAT_EDX_PAE){
        Screen::write_s("CPUID_FEAT_EDX_PAE\n");
    }
    if(edx&CPUID_FEAT_EDX_MCE){
        Screen::write_s("CPUID_FEAT_EDX_MCE\n");
    }
    if(edx&CPUID_FEAT_EDX_CX8){
        Screen::write_s("CPUID_FEAT_EDX_CX8\n");
    }
    if(edx&CPUID_FEAT_EDX_APIC){
        Screen::write_s("CPUID_FEAT_EDX_APIC\n");
    }
    if(edx&CPUID_FEAT_EDX_SEP){
        Screen::write_s("CPUID_FEAT_EDX_SEP\n");
    }
    if(edx&CPUID_FEAT_EDX_MTRR){
        Screen::write_s("CPUID_FEAT_EDX_MTRR\n");
    }
    if(edx&CPUID_FEAT_EDX_PGE){
        Screen::write_s("CPUID_FEAT_EDX_PGE\n");
    }
    if(edx&CPUID_FEAT_EDX_MCA){
        Screen::write_s("CPUID_FEAT_EDX_MCA\n");
    }
    if(edx&CPUID_FEAT_EDX_CMOV){
        Screen::write_s("CPUID_FEAT_EDX_CMOV\n");
    }
    if(edx&CPUID_FEAT_EDX_PAT){
        Screen::write_s("CPUID_FEAT_EDX_PAT\n");
    }
    if(edx&CPUID_FEAT_EDX_PSE36){
        Screen::write_s("CPUID_FEAT_EDX_PSE36\n");
    }
    if(edx&CPUID_FEAT_EDX_PSN){
        Screen::write_s("CPUID_FEAT_EDX_PSN\n");
    }
    if(edx&CPUID_FEAT_EDX_CLF){
        Screen::write_s("CPUID_FEAT_EDX_CLF\n");
    }
    if(edx&CPUID_FEAT_EDX_DTES){
        Screen::write_s("CPUID_FEAT_EDX_DTES\n");
    }
    if(edx&CPUID_FEAT_EDX_ACPI){
        Screen::write_s("CPUID_FEAT_EDX_ACPI\n");
    }
    if(edx&CPUID_FEAT_EDX_MMX){
        Screen::write_s("CPUID_FEAT_EDX_MMX\n");
    }
    if(edx&CPUID_FEAT_EDX_FXSR){
        Screen::write_s("CPUID_FEAT_EDX_FXSR\n");
    }
    if(edx&CPUID_FEAT_EDX_SSE){
        Screen::write_s("CPUID_FEAT_EDX_SSE\n");
    }
    if(edx&CPUID_FEAT_EDX_SSE2){
        Screen::write_s("CPUID_FEAT_EDX_SSE2\n");
    }
    if(edx&CPUID_FEAT_EDX_SS){
        Screen::write_s("CPUID_FEAT_EDX_SS\n");
    }
    if(edx&CPUID_FEAT_EDX_HTT){
        Screen::write_s("CPUID_FEAT_EDX_HTT\n");
    }
    if(edx&CPUID_FEAT_EDX_TM1){
        Screen::write_s("CPUID_FEAT_EDX_TM1\n");
    }
    if(edx&CPUID_FEAT_EDX_IA64){
        Screen::write_s("CPUID_FEAT_EDX_IA64\n");
    }
    if(edx&CPUID_FEAT_EDX_PBE){
        Screen::write_s("CPUID_FEAT_EDX_PBE\n");
    }
}

void CPUID::check(){
    uint32_t ecx=0,edx=0;
    asm volatile("push %0\npush %1\ncall cpuid_features"::"r"(&ecx),"r"(&edx));
    //print_features(ecx,edx);
    if(!(edx&CPUID_FEAT_EDX_APIC)){
        k_abort_s("APIC not supported");
    }
}
