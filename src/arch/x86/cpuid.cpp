#include "arch/x86.h"
#include "klib.h"
#include "screen.h"

enum cpuid_features_ecx{
    CPUID_FEAT_ECX_SSE3         = 1U <<  0,//SSE3
    CPUID_FEAT_ECX_PCLMUL       = 1U <<  1,//PCLMULQDQ
    CPUID_FEAT_ECX_DTES64       = 1U <<  2,//64-bit debug store
    CPUID_FEAT_ECX_MONITOR      = 1U <<  3,//MONITOR, MWAIT (SSE3)
    CPUID_FEAT_ECX_DS_CPL       = 1U <<  4,//CPL-qualified debug store
    CPUID_FEAT_ECX_VMX          = 1U <<  5,//Virtual Machine eXtensions
    CPUID_FEAT_ECX_SMX          = 1U <<  6,//Safer Mode Extensions
    CPUID_FEAT_ECX_EST          = 1U <<  7,//Enhanced SpeedStep
    CPUID_FEAT_ECX_TM2          = 1U <<  8,//Thermal Monitor 2
    CPUID_FEAT_ECX_SSSE3        = 1U <<  9,//Supplemental SSE3
    CPUID_FEAT_ECX_CNTX_ID      = 1U << 10,//L1 Context ID
    CPUID_FEAT_ECX_SDBG         = 1U << 11,//Silicon Debug Interface
    CPUID_FEAT_ECX_FMA          = 1U << 12,//Fused Multiply-Add (FMA3)
    CPUID_FEAT_ECX_CX16         = 1U << 13,//CMPXCHG16B compare-and-swap
    CPUID_FEAT_ECX_XTPR         = 1U << 14,//Disable task priority messages
    CPUID_FEAT_ECX_PDCM         = 1U << 15,//Perfmon & Debug Capability
    //RESERVED
    CPUID_FEAT_ECX_PCID         = 1U << 17,//process context identifiers (CR4 bit 17)
    CPUID_FEAT_ECX_DCA          = 1U << 18,//Direct cahce access for DMA
    CPUID_FEAT_ECX_SSE4_1       = 1U << 19,//SSE4.1
    CPUID_FEAT_ECX_SSE4_2       = 1U << 20,//SSE4.2
    CPUID_FEAT_ECX_x2APIC       = 1U << 21,//x2APIC
    CPUID_FEAT_ECX_MOVBE        = 1U << 22,//MOVBE (big-endian)
    CPUID_FEAT_ECX_POPCNT       = 1U << 23,//POPCNT
    CPUID_FEAT_ECX_TSC_DEADLINE = 1U << 24,//APIC TSC deadline
    CPUID_FEAT_ECX_AES          = 1U << 25,//AES
    CPUID_FEAT_ECX_XSAVE        = 1U << 26,//XSAVE,XRESTOR,XSETBV,XGETBV
    CPUID_FEAT_ECX_OSXSAVE      = 1U << 27,//XSAVE enabled by OS
    CPUID_FEAT_ECX_AVX          = 1U << 28,//Advanced Vector Extensions
    CPUID_FEAT_ECX_F16C         = 1U << 29,//Half-precision float support
    CPUID_FEAT_ECX_RDRAND       = 1U << 30,//RDRAND
    CPUID_FEAT_ECX_HYPERVISOR   = 1U << 31,//Running under hypervisor
};

const char * cpuid_features_ecx_str[] {
    "CPUID_FEAT_ECX_SSE3 -- SSE3",
    "CPUID_FEAT_ECX_PCLMUL -- PCLMULQDQ",
    "CPUID_FEAT_ECX_DTES64 -- 64-bit debug store",
    "CPUID_FEAT_ECX_MONITOR -- MONITOR, MWAIT (SSE3)",
    "CPUID_FEAT_ECX_DS_CPL -- CPL-qualified debug store",
    "CPUID_FEAT_ECX_VMX -- Virtual Machine eXtensions",
    "CPUID_FEAT_ECX_SMX -- Safer Mode Extensions",
    "CPUID_FEAT_ECX_EST -- Enhanced SpeedStep",
    "CPUID_FEAT_ECX_TM2 -- Thermal Monitor 2",
    "CPUID_FEAT_ECX_SSSE3 -- Supplemental SSE3",
    "CPUID_FEAT_ECX_CNTX_ID -- L1 Context ID",
    "CPUID_FEAT_ECX_SDBG -- Silicon Debug Interface",
    "CPUID_FEAT_ECX_FMA -- Fused Multiply-Add (FMA3)",
    "CPUID_FEAT_ECX_CX16 -- CMPXCHG16B compare-and-swap",
    "CPUID_FEAT_ECX_XTPR -- Disable task priority messages",
    "CPUID_FEAT_ECX_PDCM -- Perfmon & Debug Capability",
    "reserved -- unused",
    "CPUID_FEAT_ECX_PCID -- process context identifiers (CR4 bit 17)",
    "CPUID_FEAT_ECX_DCA -- Direct cahce access for DMA",
    "CPUID_FEAT_ECX_SSE4_1 -- SSE4.1",
    "CPUID_FEAT_ECX_SSE4_2 -- SSE4.2",
    "CPUID_FEAT_ECX_x2APIC -- x2APIC",
    "CPUID_FEAT_ECX_MOVBE -- MOVBE (big-endian)",
    "CPUID_FEAT_ECX_POPCNT -- POPCNT",
    "CPUID_FEAT_ECX_TSC_DEADLINE -- APIC TSC deadline",
    "CPUID_FEAT_ECX_AES -- AES",
    "CPUID_FEAT_ECX_XSAVE -- XSAVE,XRESTOR,XSETBV,XGETBV",
    "CPUID_FEAT_ECX_OSXSAVE -- XSAVE enabled by OS",
    "CPUID_FEAT_ECX_AVX -- Advanced Vector Extensions",
    "CPUID_FEAT_ECX_F16C -- Half-precision float support",
    "CPUID_FEAT_ECX_RDRAND -- RDRAND",
    "CPUID_FEAT_ECX_HYPERVISOR -- Running under hypervisor",
};

enum cpuid_features_edx{
    CPUID_FEAT_EDX_FPU          = 1U <<  0,//on-board FPU
    CPUID_FEAT_EDX_VME          = 1U <<  1,//virtual 8086 mode extensions
    CPUID_FEAT_EDX_DE           = 1U <<  2,//debugging extensions
    CPUID_FEAT_EDX_PSE          = 1U <<  3,//page size extension
    CPUID_FEAT_EDX_TSC          = 1U <<  4,//time stamp counter
    CPUID_FEAT_EDX_MSR          = 1U <<  5,//model-specific registers
    CPUID_FEAT_EDX_PAE          = 1U <<  6,//physical address extension
    CPUID_FEAT_EDX_MCE          = 1U <<  7,//machine check exception
    CPUID_FEAT_EDX_CX8          = 1U <<  8,//CMPXCHG8 compare-and-swap
    CPUID_FEAT_EDX_APIC         = 1U <<  9,//on-board APIC
    //RESERVED
    CPUID_FEAT_EDX_SEP          = 1U << 11,//SYSENTER & SYSEXIT
    CPUID_FEAT_EDX_MTRR         = 1U << 12,//Memory type range registers
    CPUID_FEAT_EDX_PGE          = 1U << 13,//Page global enable bit (in CR4)
    CPUID_FEAT_EDX_MCA          = 1U << 14,//Machine check architecture
    CPUID_FEAT_EDX_CMOV         = 1U << 15,//Conditional move
    CPUID_FEAT_EDX_PAT          = 1U << 16,//Page attribute table
    CPUID_FEAT_EDX_PSE_36       = 1U << 17,//36-bit page extension
    CPUID_FEAT_EDX_PSN          = 1U << 18,//Processor serial number
    CPUID_FEAT_EDX_CLFSH        = 1U << 19,//CLFLUSH (SSE2)
    //RESERVED
    CPUID_FEAT_EDX_DTES         = 1U << 21,//Debug store
    CPUID_FEAT_EDX_ACPI         = 1U << 22,//ACPI thermal control MSRs
    CPUID_FEAT_EDX_MMX          = 1U << 23,//MMX
    CPUID_FEAT_EDX_FXSR         = 1U << 24,//FXSAVE,FXRESTOR, CR4 bit 9
    CPUID_FEAT_EDX_SSE          = 1U << 25,//SSE
    CPUID_FEAT_EDX_SSE2         = 1U << 26,//SSE2
    CPUID_FEAT_EDX_SS           = 1U << 27,//CPU cache self-snoop
    CPUID_FEAT_EDX_HTT          = 1U << 28,//Hyper-threading
    CPUID_FEAT_EDX_TM1          = 1U << 29,//Thermal Monitor 1
    CPUID_FEAT_EDX_IA64         = 1U << 30,//Running as x86 on 64-bit CPU
    CPUID_FEAT_EDX_PBE          = 1U << 31,//Pending Break Enable wakeup capability
};

const char * cpuid_features_edx_str[] {
    "CPUID_FEAT_EDX_FPU -- on-board FPU",
    "CPUID_FEAT_EDX_VME -- virtual 8086 mode extensions",
    "CPUID_FEAT_EDX_DE -- debugging extensions",
    "CPUID_FEAT_EDX_PSE -- page size extension",
    "CPUID_FEAT_EDX_TSC -- time stamp counter",
    "CPUID_FEAT_EDX_MSR -- model-specific registers",
    "CPUID_FEAT_EDX_PAE -- physical address extension",
    "CPUID_FEAT_EDX_MCE -- machine check exception",
    "CPUID_FEAT_EDX_CX8 -- CMPXCHG8 compare-and-swap",
    "CPUID_FEAT_EDX_APIC -- on-board APIC",
    "reserved -- unused",
    "CPUID_FEAT_EDX_SEP -- SYSENTER & SYSEXIT",
    "CPUID_FEAT_EDX_MTRR -- Memory type range registers",
    "CPUID_FEAT_EDX_PGE -- Page global enable bit (in CR4)",
    "CPUID_FEAT_EDX_MCA -- Machine check architecture",
    "CPUID_FEAT_EDX_CMOV -- Conditional move",
    "CPUID_FEAT_EDX_PAT -- Page attribute table",
    "CPUID_FEAT_EDX_PSE_36 -- 36-bit page extension",
    "CPUID_FEAT_EDX_PSN -- Processor serial number",
    "CPUID_FEAT_EDX_CLFSH -- CLFLUSH (SSE2)",
    "reserved -- unused",
    "CPUID_FEAT_EDX_DTES -- Debug store",
    "CPUID_FEAT_EDX_ACPI -- ACPI thermal control MSRs",
    "CPUID_FEAT_EDX_MMX -- MMX",
    "CPUID_FEAT_EDX_FXSR -- FXSAVE,FXRESTOR, CR4 bit 9",
    "CPUID_FEAT_EDX_SSE -- SSE",
    "CPUID_FEAT_EDX_SSE2 -- SSE2",
    "CPUID_FEAT_EDX_SS -- CPU cache self-snoop",
    "CPUID_FEAT_EDX_HTT -- Hyper-threading",
    "CPUID_FEAT_EDX_TM1 -- Thermal Monitor 1",
    "CPUID_FEAT_EDX_IA64 -- Running as x86 on 64-bit CPU",
    "CPUID_FEAT_EDX_PBE -- Pending Break Enable wakeup capability",
};

void print_features(uint32_t ecx,uint32_t edx){
    //k_cls();
    k_putc('\n');
    uint32_t lines=k_get_y();
    if(lines>20){
        k_cls();
        lines=0;
    }
    Screen::setfgcolor(Screen::LIGHT_CYAN);
    k_puts("ECX\n");
    Screen::setfgcolor(Screen::WHITE);
    lines++;
    for(uint32_t i=0;i<32;i++){
        if(ecx&(1<<i)){
            if(lines==24){
                k_puts("press any key to continue...");
                k_getch_extended();
                k_cls();
                lines=0;
            }
            lines++;
            k_puts(cpuid_features_ecx_str[i]);
            k_putc('\n');
        }
    }
    if(lines>20){
        k_puts("press any key to continue...");
        k_getch_extended();
        k_cls();
        lines=0;
    }
    Screen::setfgcolor(Screen::LIGHT_CYAN);
    k_puts("EDX\n");
    Screen::setfgcolor(Screen::WHITE);
    lines++;
    for(uint32_t i=0;i<32;i++){
        if(edx&(1<<i)){
            if(lines==24){
                k_puts("press any key to continue...");
                k_getch_extended();
                k_cls();
                lines=0;
            }
            lines++;
            k_puts(cpuid_features_edx_str[i]);
            k_putc('\n');
        }
    }
}

void CPUID::cmd_cpuid(){
    uint32_t ecx=0,edx=0;
    asm volatile("push %0\npush %1\ncall cpuid_features"::"r"(&ecx),"r"(&edx));
    print_features(ecx,edx);
}

void CPUID::check(){
    uint32_t ecx=0,edx=0;
    asm volatile("push %0\npush %1\ncall cpuid_features"::"r"(&ecx),"r"(&edx));
    if(!(edx&CPUID_FEAT_EDX_MSR)){
        k_abort_s("MSR not supported");
    }
    if(!(edx&CPUID_FEAT_EDX_APIC)){
        k_abort_s("APIC not supported");
    }
}
