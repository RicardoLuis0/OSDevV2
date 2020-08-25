#ifndef X86_CPUID_H_INCLUDED
#define X86_CPUID_H_INCLUDED

#include <stdint.h>

namespace CPUID {
    
    extern "C" uint32_t has_cpuid();
    
    enum features_ecx{
        FEAT_ECX_SSE3         = 1U <<  0,//SSE3
        FEAT_ECX_PCLMUL       = 1U <<  1,//PCLMULQDQ
        FEAT_ECX_DTES64       = 1U <<  2,//64-bit debug store
        FEAT_ECX_MONITOR      = 1U <<  3,//MONITOR, MWAIT (SSE3)
        FEAT_ECX_DS_CPL       = 1U <<  4,//CPL-qualified debug store
        FEAT_ECX_VMX          = 1U <<  5,//Virtual Machine eXtensions
        FEAT_ECX_SMX          = 1U <<  6,//Safer Mode Extensions
        FEAT_ECX_EST          = 1U <<  7,//Enhanced SpeedStep
        FEAT_ECX_TM2          = 1U <<  8,//Thermal Monitor 2
        FEAT_ECX_SSSE3        = 1U <<  9,//Supplemental SSE3
        FEAT_ECX_CNTX_ID      = 1U << 10,//L1 Context ID
        FEAT_ECX_SDBG         = 1U << 11,//Silicon Debug Interface
        FEAT_ECX_FMA          = 1U << 12,//Fused Multiply-Add (FMA3)
        FEAT_ECX_CX16         = 1U << 13,//CMPXCHG16B compare-and-swap
        FEAT_ECX_XTPR         = 1U << 14,//Disable task priority messages
        FEAT_ECX_PDCM         = 1U << 15,//Perfmon & Debug Capability
        //bit 16 reserved
        FEAT_ECX_PCID         = 1U << 17,//process context identifiers (CR4 bit 17)
        FEAT_ECX_DCA          = 1U << 18,//Direct cahce access for DMA
        FEAT_ECX_SSE4_1       = 1U << 19,//SSE4.1
        FEAT_ECX_SSE4_2       = 1U << 20,//SSE4.2
        FEAT_ECX_x2APIC       = 1U << 21,//x2APIC
        FEAT_ECX_MOVBE        = 1U << 22,//MOVBE (big-endian)
        FEAT_ECX_POPCNT       = 1U << 23,//POPCNT
        FEAT_ECX_TSC_DEADLINE = 1U << 24,//APIC TSC deadline
        FEAT_ECX_AES          = 1U << 25,//AES
        FEAT_ECX_XSAVE        = 1U << 26,//XSAVE,XRESTOR,XSETBV,XGETBV
        FEAT_ECX_OSXSAVE      = 1U << 27,//XSAVE enabled by OS
        FEAT_ECX_AVX          = 1U << 28,//Advanced Vector Extensions
        FEAT_ECX_F16C         = 1U << 29,//Half-precision float support
        FEAT_ECX_RDRAND       = 1U << 30,//RDRAND
        FEAT_ECX_HYPERVISOR   = 1U << 31,//Running under hypervisor
    };
    
    enum features_edx{
        FEAT_EDX_FPU          = 1U <<  0,//on-board FPU
        FEAT_EDX_VME          = 1U <<  1,//virtual 8086 mode extensions
        FEAT_EDX_DE           = 1U <<  2,//debugging extensions
        FEAT_EDX_PSE          = 1U <<  3,//page size extension
        FEAT_EDX_TSC          = 1U <<  4,//time stamp counter
        FEAT_EDX_MSR          = 1U <<  5,//model-specific registers
        FEAT_EDX_PAE          = 1U <<  6,//physical address extension
        FEAT_EDX_MCE          = 1U <<  7,//machine check exception
        FEAT_EDX_CX8          = 1U <<  8,//CMPXCHG8 compare-and-swap
        FEAT_EDX_APIC         = 1U <<  9,//on-board APIC
        //bit 10 reserved
        FEAT_EDX_SEP          = 1U << 11,//SYSENTER & SYSEXIT
        FEAT_EDX_MTRR         = 1U << 12,//Memory type range registers
        FEAT_EDX_PGE          = 1U << 13,//Page global enable bit (in CR4)
        FEAT_EDX_MCA          = 1U << 14,//Machine check architecture
        FEAT_EDX_CMOV         = 1U << 15,//Conditional move
        FEAT_EDX_PAT          = 1U << 16,//Page attribute table
        FEAT_EDX_PSE_36       = 1U << 17,//36-bit page extension
        FEAT_EDX_PSN          = 1U << 18,//Processor serial number
        FEAT_EDX_CLFSH        = 1U << 19,//CLFLUSH (SSE2)
        //bit 20 reserved
        FEAT_EDX_DTES         = 1U << 21,//Debug store
        FEAT_EDX_ACPI         = 1U << 22,//ACPI thermal control MSRs
        FEAT_EDX_MMX          = 1U << 23,//MMX
        FEAT_EDX_FXSR         = 1U << 24,//FXSAVE,FXRESTOR, CR4 bit 9
        FEAT_EDX_SSE          = 1U << 25,//SSE
        FEAT_EDX_SSE2         = 1U << 26,//SSE2
        FEAT_EDX_SS           = 1U << 27,//CPU cache self-snoop
        FEAT_EDX_HTT          = 1U << 28,//Hyper-threading
        FEAT_EDX_TM1          = 1U << 29,//Thermal Monitor 1
        FEAT_EDX_IA64         = 1U << 30,//Running as x86 emulation on 64-bit CPU
        FEAT_EDX_PBE          = 1U << 31,//Pending Break Enable wakeup capability
    };
    void check();
    void get(uint32_t &ecx,uint32_t &edx);
    bool has(uint32_t ecx,uint32_t edx);
    void cmd_cpuid();
    
}


#endif // X86_CPUID_H_INCLUDED
