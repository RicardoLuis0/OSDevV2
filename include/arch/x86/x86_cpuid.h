#ifndef X86_CPUID_H_INCLUDED
#define X86_CPUID_H_INCLUDED

#include <stdint.h>

namespace CPUID {
    
    bool has_cpuid();
    
    enum features_ecx_1{
        FEAT_ECX_1_SSE3         = 1U <<  0,//SSE3
        FEAT_ECX_1_PCLMUL       = 1U <<  1,//PCLMULQDQ
        FEAT_ECX_1_DTES64       = 1U <<  2,//64-bit debug store
        FEAT_ECX_1_MONITOR      = 1U <<  3,//MONITOR, MWAIT (SSE3)
        FEAT_ECX_1_DS_CPL       = 1U <<  4,//CPL-qualified debug store
        FEAT_ECX_1_VMX          = 1U <<  5,//Virtual Machine eXtensions
        FEAT_ECX_1_SMX          = 1U <<  6,//Safer Mode Extensions
        FEAT_ECX_1_EST          = 1U <<  7,//Enhanced SpeedStep
        FEAT_ECX_1_TM2          = 1U <<  8,//Thermal Monitor 2
        FEAT_ECX_1_SSSE3        = 1U <<  9,//Supplemental SSE3
        FEAT_ECX_1_CNTX_ID      = 1U << 10,//L1 Context ID
        FEAT_ECX_1_SDBG         = 1U << 11,//Silicon Debug Interface
        FEAT_ECX_1_FMA          = 1U << 12,//Fused Multiply-Add (FMA3)
        FEAT_ECX_1_CX16         = 1U << 13,//CMPXCHG16B compare-and-swap
        FEAT_ECX_1_XTPR         = 1U << 14,//Disable task priority messages
        FEAT_ECX_1_PDCM         = 1U << 15,//Perfmon & Debug Capability
        //BIT 16 RESERVED
        FEAT_ECX_1_PCID         = 1U << 17,//process context identifiers (CR4 bit 17)
        FEAT_ECX_1_DCA          = 1U << 18,//Direct cahce access for DMA
        FEAT_ECX_1_SSE4_1       = 1U << 19,//SSE4.1
        FEAT_ECX_1_SSE4_2       = 1U << 20,//SSE4.2
        FEAT_ECX_1_x2APIC       = 1U << 21,//x2APIC
        FEAT_ECX_1_MOVBE        = 1U << 22,//MOVBE (big-endian)
        FEAT_ECX_1_POPCNT       = 1U << 23,//POPCNT
        FEAT_ECX_1_TSC_DEADLINE = 1U << 24,//APIC TSC deadline
        FEAT_ECX_1_AES          = 1U << 25,//AES
        FEAT_ECX_1_XSAVE        = 1U << 26,//XSAVE,XRESTOR,XSETBV,XGETBV
        FEAT_ECX_1_OSXSAVE      = 1U << 27,//XSAVE enabled by OS
        FEAT_ECX_1_AVX          = 1U << 28,//Advanced Vector Extensions
        FEAT_ECX_1_F16C         = 1U << 29,//Half-precision float support
        FEAT_ECX_1_RDRAND       = 1U << 30,//RDRAND
        FEAT_ECX_1_HYPERVISOR   = 1U << 31,//Running under hypervisor
    };
    
    enum features_edx_1{
        FEAT_EDX_1_FPU          = 1U <<  0,//on-board FPU
        FEAT_EDX_1_VME          = 1U <<  1,//virtual 8086 mode extensions
        FEAT_EDX_1_DE           = 1U <<  2,//debugging extensions
        FEAT_EDX_1_PSE          = 1U <<  3,//page size extension
        FEAT_EDX_1_TSC          = 1U <<  4,//time stamp counter
        FEAT_EDX_1_MSR          = 1U <<  5,//model-specific registers
        FEAT_EDX_1_PAE          = 1U <<  6,//physical address extension
        FEAT_EDX_1_MCE          = 1U <<  7,//machine check exception
        FEAT_EDX_1_CX8          = 1U <<  8,//CMPXCHG8 compare-and-swap
        FEAT_EDX_1_APIC         = 1U <<  9,//on-board APIC
        //BIT 10 RESERVED
        FEAT_EDX_1_SEP          = 1U << 11,//SYSENTER & SYSEXIT
        FEAT_EDX_1_MTRR         = 1U << 12,//Memory type range registers
        FEAT_EDX_1_PGE          = 1U << 13,//Page global enable bit (in CR4)
        FEAT_EDX_1_MCA          = 1U << 14,//Machine check architecture
        FEAT_EDX_1_CMOV         = 1U << 15,//Conditional move
        FEAT_EDX_1_PAT          = 1U << 16,//Page attribute table
        FEAT_EDX_1_PSE_36       = 1U << 17,//36-bit page extension
        FEAT_EDX_1_PSN          = 1U << 18,//Processor serial number
        FEAT_EDX_1_CLFSH        = 1U << 19,//CLFLUSH (SSE2)
        //BIT 20 RESERVED
        FEAT_EDX_1_DTES         = 1U << 21,//Debug store
        FEAT_EDX_1_ACPI         = 1U << 22,//ACPI thermal control MSRs
        FEAT_EDX_1_MMX          = 1U << 23,//MMX
        FEAT_EDX_1_FXSR         = 1U << 24,//FXSAVE,FXRESTOR, CR4 bit 9
        FEAT_EDX_1_SSE          = 1U << 25,//SSE
        FEAT_EDX_1_SSE2         = 1U << 26,//SSE2
        FEAT_EDX_1_SS           = 1U << 27,//CPU cache self-snoop
        FEAT_EDX_1_HTT          = 1U << 28,//Hyper-threading
        FEAT_EDX_1_TM1          = 1U << 29,//Thermal Monitor 1
        FEAT_EDX_1_IA64         = 1U << 30,//Running as x86 emulation on 64-bit CPU
        FEAT_EDX_1_PBE          = 1U << 31,//Pending Break Enable wakeup capability
    };
    
    enum features_ebx_7_0 {
        FEAT_EBX_7_0_FSGSBASE           = 1U <<  0,
        FEAT_EBX_7_0_IA32_TSC_ADJUST    = 1U <<  1,
        FEAT_EBX_7_0_SGX                = 1U <<  2,
        FEAT_EBX_7_0_BMI1               = 1U <<  3,
        FEAT_EBX_7_0_HLE                = 1U <<  4,
        FEAT_EBX_7_0_AVX2               = 1U <<  5,
        FEAT_EBX_7_0_FDP_EXCPTN_ONLY    = 1U <<  6,
        FEAT_EBX_7_0_SMEP               = 1U <<  7,
        FEAT_EBX_7_0_BMI2               = 1U <<  8,
        FEAT_EBX_7_0_ERMS               = 1U <<  9,
        FEAT_EBX_7_0_INVPCID            = 1U << 10,
        FEAT_EBX_7_0_RTM                = 1U << 11,
        FEAT_EBX_7_0_PQM                = 1U << 12,
        FEAT_EBX_7_0_FPU_CSDS_DPC       = 1U << 13,
        FEAT_EBX_7_0_MPX                = 1U << 14,
        FEAT_EBX_7_0_PQE                = 1U << 15,
        FEAT_EBX_7_0_AVX512F            = 1U << 16,
        FEAT_EBX_7_0_AVX512DQ           = 1U << 17,
        FEAT_EBX_7_0_RDSEED             = 1U << 18,
        FEAT_EBX_7_0_ADX                = 1U << 19,
        FEAT_EBX_7_0_SMAP               = 1U << 20,
        FEAT_EBX_7_0_AVX512IFMA         = 1U << 21,
        FEAT_EBX_7_0_PCOMMIT            = 1U << 22,
        FEAT_EBX_7_0_CLFLUSHOPT         = 1U << 23,
        FEAT_EBX_7_0_CLWB               = 1U << 24,
        FEAT_EBX_7_0_INTEL_PT           = 1U << 25,
        FEAT_EBX_7_0_AVX512PF           = 1U << 26,
        FEAT_EBX_7_0_AVX512ER           = 1U << 27,
        FEAT_EBX_7_0_AVX512CD           = 1U << 28,
        FEAT_EBX_7_0_SHA                = 1U << 29,
        FEAT_EBX_7_0_AVX512BW           = 1U << 30,
        FEAT_EBX_7_0_AVX512VL           = 1U << 31,
    };
    
    enum features_ecx_7_0 {
        FEAT_ECX_7_0_PREFETCHWT1        = 1U <<  0,
        FEAT_ECX_7_0_AVX512VBMI         = 1U <<  1,
        FEAT_ECX_7_0_UMIP               = 1U <<  2,
        FEAT_ECX_7_0_PKU                = 1U <<  3,
        FEAT_ECX_7_0_OSPKE              = 1U <<  4,
        FEAT_ECX_7_0_WAITPKG            = 1U <<  5,
        FEAT_ECX_7_0_AVC512VBMI2        = 1U <<  6,
        FEAT_ECX_7_0_CET_SS             = 1U <<  7,
        FEAT_ECX_7_0_GFNI               = 1U <<  8,
        FEAT_ECX_7_0_VAES               = 1U <<  9,
        FEAT_ECX_7_0_VPCLMULQDQ         = 1U << 10,
        FEAT_ECX_7_0_AVX512VNNI         = 1U << 11,
        FEAT_ECX_7_0_AVX512BITALG       = 1U << 12,
        //BIT 13 RESERVED
        FEAT_ECX_7_0_AVX512VPOPCNTDQ    = 1U << 14,
        //BIT 15 RESERVED
        FEAT_ECX_7_0_5LVPG              = 1U << 16,
        //BIT 17 MAWAU
        //BIT 18 MAWAU
        //BIT 19 MAWAU
        //BIT 20 MAWAU
        //BIT 21 MAWAU
        FEAT_ECX_7_0_RDPID              = 1U << 22,
        //BIT 23 RESERVED
        //BIT 24 RESERVED
        FEAT_ECX_7_0_CLDEMOTE           = 1U << 25,
        //BIT 26 RESERVED
        FEAT_ECX_7_0_MOVDIRI            = 1U << 27,
        FEAT_ECX_7_0_MOVDIR64B          = 1U << 28,
        FEAT_ECX_7_0_ENQCMD             = 1U << 29,
        FEAT_ECX_7_0_SGX_LC             = 1U << 30,
        FEAT_ECX_7_0_PKS                = 1U << 31,
    };
    
    enum features_edx_7_0 {
        //BIT  0 RESERVED
        //BIT  1 RESERVED
        FEAT_EDX_7_0_AVC512_4VNNIW      = 1U <<  2,
        FEAT_EDX_7_0_AVC512_4FMAPS      = 1U <<  3,
        FEAT_EDX_7_0_FSRM               = 1U <<  4,
        //BIT  5 RESERVED
        //BIT  6 RESERVED
        //BIT  7 RESERVED
        FEAT_EDX_7_0_AVC512VP2INTERSECT = 1U <<  8,
        FEAT_EDX_7_0_SRBDS_CTRL         = 1U <<  9,
        FEAT_EDX_7_0_MD_CLEAR           = 1U << 10,
        //BIT 11 RESERVED
        //BIT 12 RESERVED
        FEAT_EDX_7_0_TSX_FORCE_ABORT    = 1U << 13,
        FEAT_EDX_7_0_SERIALIZE          = 1U << 14,
        FEAT_EDX_7_0_HYBRID             = 1U << 15,
        FEAT_EDX_7_0_TSXLDTRK           = 1U << 16,
        //BIT 17 RESERVED
        FEAT_EDX_7_0_PCONFIG            = 1U << 18,
        FEAT_EDX_7_0_LBR                = 1U << 19,
        FEAT_EDX_7_0_CET_IBT            = 1U << 20,
        //BIT 21 RESERVED
        FEAT_EDX_7_0_AMXBF16            = 1U << 22,
        //BIT 23 RESERVED
        FEAT_EDX_7_0_AMXTILE            = 1U << 24,
        FEAT_EDX_7_0_AMXINT8            = 1U << 25,
        FEAT_EDX_7_0_IBRS_IBPB          = 1U << 26,
        FEAT_EDX_7_0_STIBP              = 1U << 27,
        FEAT_EDX_7_0_L1D_FLUSH          = 1U << 28,
        FEAT_EDX_7_0_IA32_ARCH_CAP      = 1U << 29,
        FEAT_EDX_7_0_IA32_CORE_CAP      = 1U << 30,
        FEAT_EDX_7_0_SSBD               = 1U << 31,
    };
    
    enum features_eax_7_1 {
        //BIT  0 RESERVED
        //BIT  1 RESERVED
        //BIT  2 RESERVED
        //BIT  3 RESERVED
        //BIT  4 RESERVED
        FEAT_EAX_7_1_AVX512BF16         = 1U <<  5,
        //BIT  6 RESERVED
        //BIT  7 RESERVED
        //BIT  8 RESERVED
        //BIT  9 RESERVED
        //BIT 10 RESERVED
        //BIT 11 RESERVED
        //BIT 12 RESERVED
        //BIT 13 RESERVED
        //BIT 14 RESERVED
        //BIT 15 RESERVED
        //BIT 16 RESERVED
        //BIT 17 RESERVED
        //BIT 18 RESERVED
        //BIT 19 RESERVED
        //BIT 20 RESERVED
        //BIT 21 RESERVED
        //BIT 22 RESERVED
        //BIT 23 RESERVED
        //BIT 24 RESERVED
        //BIT 25 RESERVED
        //BIT 26 RESERVED
        //BIT 27 RESERVED
        //BIT 28 RESERVED
        //BIT 29 RESERVED
        //BIT 30 RESERVED
        //BIT 31 RESERVED
    };
    
    void init();//safe to call multiple times, will refresh cpuid info
    void check();
    void get(uint32_t &ecx_1,uint32_t &edx_1);
    void get(uint32_t &ecx_1,uint32_t &edx_1,uint32_t &ebx_7_0,uint32_t &ecx_7_0,uint32_t &edx_7_0,uint32_t &eax_7_1);
    bool has(uint32_t ecx_1,uint32_t edx_1);
    bool has(uint32_t ecx_1,uint32_t edx_1,uint32_t ebx_7_0,uint32_t ecx_7_0,uint32_t edx_7_0,uint32_t eax_7_1);
    void cmd_cpuid();
    
}


#endif // X86_CPUID_H_INCLUDED
