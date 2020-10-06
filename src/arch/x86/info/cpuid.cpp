#include "arch/x86.h"
#include "klib.h"
#include "screen.h"

using namespace CPUID;

static const char * reserved_str="reserved            -- unused";

static const char * features_ecx_1_str[32] {
    "ECX_1_SSE3          -- SSE3",
    "ECX_1_PCLMUL        -- PCLMULQDQ",
    "ECX_1_DTES64        -- 64-bit debug store",
    "ECX_1_MONITOR       -- MONITOR, MWAIT (SSE3)",
    "ECX_1_DS_CPL        -- CPL-qualified debug store",
    "ECX_1_VMX           -- Virtual Machine eXtensions",
    "ECX_1_SMX           -- Safer Mode Extensions",
    "ECX_1_EST           -- Enhanced SpeedStep",
    "ECX_1_TM2           -- Thermal Monitor 2",
    "ECX_1_SSSE3         -- Supplemental SSE3",
    "ECX_1_CNTX_ID       -- L1 Context ID",
    "ECX_1_SDBG          -- Silicon Debug Interface",
    "ECX_1_FMA           -- Fused Multiply-Add (FMA3)",
    "ECX_1_CX16          -- CMPXCHG16B compare-and-swap",
    "ECX_1_XTPR          -- Disable task priority messages",
    "ECX_1_PDCM          -- Perfmon & Debug Capability",
    reserved_str,
    "ECX_1_PCID          -- process context identifiers (CR4 bit 17)",
    "ECX_1_DCA           -- Direct cahce access for DMA",
    "ECX_1_SSE4_1        -- SSE4.1",
    "ECX_1_SSE4_2        -- SSE4.2",
    "ECX_1_x2APIC        -- x2APIC",
    "ECX_1_MOVBE         -- MOVBE (big-endian)",
    "ECX_1_POPCNT        -- POPCNT",
    "ECX_1_TSC_DEADLINE  -- APIC TSC deadline",
    "ECX_1_AES           -- AES",
    "ECX_1_XSAVE         -- XSAVE,XRESTOR,XSETBV,XGETBV",
    "ECX_1_OSXSAVE       -- XSAVE enabled by OS",
    "ECX_1_AVX           -- Advanced Vector Extensions",
    "ECX_1_F16C          -- Half-precision float support",
    "ECX_1_RDRAND        -- RDRAND",
    "ECX_1_HYPERVISOR    -- Running under hypervisor",
};

static const char * features_edx_1_str[32] {
    "EDX_1_FPU           -- on-board FPU",
    "EDX_1_VME           -- virtual 8086 mode extensions",
    "EDX_1_DE            -- debugging extensions",
    "EDX_1_PSE           -- page size extension",
    "EDX_1_TSC           -- time stamp counter",
    "EDX_1_MSR           -- model-specific registers",
    "EDX_1_PAE           -- physical address extension",
    "EDX_1_MCE           -- machine check exception",
    "EDX_1_CX8           -- CMPXCHG8 compare-and-swap",
    "EDX_1_APIC          -- on-board APIC",
    reserved_str,
    "EDX_1_SEP           -- SYSENTER & SYSEXIT",
    "EDX_1_MTRR          -- Memory type range registers",
    "EDX_1_PGE           -- Page global enable bit (in CR4)",
    "EDX_1_MCA           -- Machine check architecture",
    "EDX_1_CMOV          -- Conditional move",
    "EDX_1_PAT           -- Page attribute table",
    "EDX_1_PSE_36        -- 36-bit page extension",
    "EDX_1_PSN           -- Processor serial number",
    "EDX_1_CLFSH         -- CLFLUSH (SSE2)",
    reserved_str,
    "EDX_1_DTES          -- Debug store",
    "EDX_1_ACPI          -- ACPI thermal control MSRs",
    "EDX_1_MMX           -- MMX",
    "EDX_1_FXSR          -- FXSAVE,FXRESTOR, CR4 bit 9",
    "EDX_1_SSE           -- SSE",
    "EDX_1_SSE2          -- SSE2",
    "EDX_1_SS            -- CPU cache self-snoop",
    "EDX_1_HTT           -- Hyper-threading",
    "EDX_1_TM1           -- Thermal Monitor 1",
    "EDX_1_IA64          -- Running as x86 emulation on 64-bit CPU",
    "EDX_1_PBE           -- Pending Break Enable wakeup capability",
};


static const char * features_ebx_7_0_str[32] {
    "EBX_7_0_FSGSBASE",
    "EBX_7_0_IA32_TSC_ADJUST",
    "EBX_7_0_SGX",
    "EBX_7_0_BMI1",
    "EBX_7_0_HLE",
    "EBX_7_0_AVX2",
    "EBX_7_0_FDP_EXCPTN_ONLY",
    "EBX_7_0_SMEP",
    "EBX_7_0_BMI2",
    "EBX_7_0_ERMS",
    "EBX_7_0_INVPCID",
    "EBX_7_0_RTM",
    "EBX_7_0_PQM",
    "EBX_7_0_FPU_CSDS_DPC",
    "EBX_7_0_MPX",
    "EBX_7_0_PQE",
    "EBX_7_0_AVX512F",
    "EBX_7_0_AVX512DQ",
    "EBX_7_0_RDSEED",
    "EBX_7_0_ADX",
    "EBX_7_0_SMAP",
    "EBX_7_0_AVX512IFMA",
    "EBX_7_0_PCOMMIT",
    "EBX_7_0_CLFLUSHOPT",
    "EBX_7_0_CLWB",
    "EBX_7_0_INTEL_PT",
    "EBX_7_0_AVX512PF",
    "EBX_7_0_AVX512ER",
    "EBX_7_0_AVX512CD",
    "EBX_7_0_SHA",
    "EBX_7_0_AVX512BW",
    "EBX_7_0_AVX512VL",
};

static const char * features_ecx_7_0_str[32] {
    "ECX_7_0_PREFETCHWT1",
    "ECX_7_0_AVX512VBMI",
    "ECX_7_0_UMIP",
    "ECX_7_0_PKU",
    "ECX_7_0_OSPKE",
    "ECX_7_0_WAITPKG",
    "ECX_7_0_AVC512VBMI2",
    "ECX_7_0_CET_SS",
    "ECX_7_0_GFNI",
    "ECX_7_0_VAES",
    "ECX_7_0_VPCLMULQDQ",
    "ECX_7_0_AVX512VNNI",
    "ECX_7_0_AVX512BITALG",
    reserved_str,
    "ECX_7_0_AVX512VPOPCNTDQ",
    reserved_str,
    "ECX_7_0_5LVPG",
    reserved_str,
    reserved_str,
    reserved_str,
    reserved_str,
    reserved_str,
    "ECX_7_0_RDPID",
    reserved_str,
    reserved_str,
    "ECX_7_0_CLDEMOTE",
    reserved_str,
    "ECX_7_0_MOVDIRI",
    "ECX_7_0_MOVDIR64B",
    "ECX_7_0_ENQCMD",
    "ECX_7_0_SGX_LC",
    "ECX_7_0_PKS",
};

static const char * features_edx_7_0_str[32] {
    reserved_str,
    reserved_str,
    "EDX_7_0_AVC512_4VNNIW",
    "EDX_7_0_AVC512_4FMAPS",
    "EDX_7_0_FSRM",
    reserved_str,
    reserved_str,
    reserved_str,
    "EDX_7_0_AVC512VP2INTERSECT",
    "EDX_7_0_SRBDS_CTRL",
    "EDX_7_0_MD_CLEAR",
    reserved_str,
    reserved_str,
    "EDX_7_0_TSX_FORCE_ABORT",
    "EDX_7_0_SERIALIZE",
    "EDX_7_0_HYBRID",
    "EDX_7_0_TSXLDTRK",
    reserved_str,
    "EDX_7_0_PCONFIG",
    "EDX_7_0_LBR",
    "EDX_7_0_CET_IBT",
    reserved_str,
    "EDX_7_0_AMXBF16",
    reserved_str,
    "EDX_7_0_AMXTILE",
    "EDX_7_0_AMXINT8",
    "EDX_7_0_IBRS_IBPB",
    "EDX_7_0_STIBP",
    "EDX_7_0_L1D_FLUSH",
    "EDX_7_0_IA32_ARCH_CAP",
    "EDX_7_0_IA32_CORE_CAP",
    "EDX_7_0_SSBD",
};

static const char * features_eax_7_1_str[32] {
    reserved_str,
    reserved_str,
    reserved_str,
    reserved_str,
    reserved_str,
    "EAX_7_1_AVX512BF16",
    reserved_str,
    reserved_str,
    reserved_str,
    reserved_str,
    reserved_str,
    reserved_str,
    reserved_str,
    reserved_str,
    reserved_str,
    reserved_str,
    reserved_str,
    reserved_str,
    reserved_str,
    reserved_str,
    reserved_str,
    reserved_str,
    reserved_str,
    reserved_str,
    reserved_str,
    reserved_str,
    reserved_str,
    reserved_str,
    reserved_str,
    reserved_str,
    reserved_str,
    reserved_str,
};


namespace CPUID {
    bool b_has_cpuid;
    uint32_t eax0,ebx0,ecx0,edx0;
    uint32_t eax1,ebx1,ecx1,edx1;
    uint32_t eax7_0,ebx7_0,ecx7_0,edx7_0;
    uint32_t eax7_1,ebx7_1,ecx7_1,edx7_1;
}

namespace CPUID {
    static void print_features(){
        uint32_t lines=0;
        Screen::setfgcolor(Screen::LIGHT_CYAN);
        k_puts("\nVendor ID:\n");
        Screen::setfgcolor(Screen::WHITE);
        k_putsn(reinterpret_cast<char*>(&ebx0),4);
        k_putsn(reinterpret_cast<char*>(&edx0),4);
        k_putsn(reinterpret_cast<char*>(&ecx0),4);
        lines+=2;
        Screen::setfgcolor(Screen::LIGHT_CYAN);
        k_puts("\nHighest Function:\n");
        Screen::setfgcolor(Screen::WHITE);
        k_puth(eax0);
        lines+=2;
        Screen::setfgcolor(Screen::LIGHT_CYAN);
        k_puts("\nLAPIC ID:\n");
        Screen::setfgcolor(Screen::WHITE);
        k_puth(ebx1>>24);
        lines+=2;
        Screen::setfgcolor(Screen::LIGHT_CYAN);
        k_puts("\nECX_1");
        Screen::setfgcolor(Screen::WHITE);
        lines++;
        if(ecx1!=0){
            for(uint32_t i=0;i<32;i++){
                if(i==16)continue;//skip reserved bits
                if(ecx1&(1U<<i)){
                    if(lines>=24){
                        k_puts("\npress any key to continue...");
                        k_getch_extended();
                        lines=1;
                    }
                    lines++;
                    k_putc('\n');
                    k_puts(features_ecx_1_str[i]);
                }
            }
        }
        if(edx1!=0){
            if(lines>20){
                k_puts("\npress any key to continue...");
                k_getch_extended();
                lines=1;
            }
            Screen::setfgcolor(Screen::LIGHT_CYAN);
            k_puts("\nEDX_1");
            Screen::setfgcolor(Screen::WHITE);
            lines++;
            for(uint32_t i=0;i<32;i++){
                if(i==10||i==20)continue;//skip reserved bits
                if(edx1&(1U<<i)){
                    if(lines>=24){
                        k_puts("\npress any key to continue...");
                        k_getch_extended();
                        lines=1;
                    }
                    lines++;
                    k_putc('\n');
                    k_puts(features_edx_1_str[i]);
                }
            }
        }
        if(ebx7_0!=0){
            if(lines>20){
                k_puts("\npress any key to continue...");
                k_getch_extended();
                lines=1;
            }
            Screen::setfgcolor(Screen::LIGHT_CYAN);
            k_puts("\nEBX_7_0");
            Screen::setfgcolor(Screen::WHITE);
            lines++;
            for(uint32_t i=0;i<32;i++){
                if(ebx7_0&(1U<<i)){
                    if(lines>=24){
                        k_puts("\npress any key to continue...");
                        k_getch_extended();
                        lines=1;
                    }
                    lines++;
                    k_putc('\n');
                    k_puts(features_ebx_7_0_str[i]);
                }
            }
        }
        if(ecx7_0!=0){
            if(lines>20){
                k_puts("\npress any key to continue...");
                k_getch_extended();
                lines=1;
            }
            Screen::setfgcolor(Screen::LIGHT_CYAN);
            k_puts("\nECX_7_0");
            Screen::setfgcolor(Screen::WHITE);
            lines++;
            for(uint32_t i=0;i<32;i++){
                if(i==13||i==15||(i>=17&&i<=21)||i==23||i==24||i==26)continue;//skip reserved bits
                if(ecx7_0&(1U<<i)){
                    if(lines>=24){
                        k_puts("\npress any key to continue...");
                        k_getch_extended();
                        lines=1;
                    }
                    lines++;
                    k_putc('\n');
                    k_puts(features_ecx_7_0_str[i]);
                }
            }
            if(lines>20){
                k_puts("\npress any key to continue...");
                k_getch_extended();
                lines=1;
            }
        }
        if(edx7_0!=0){
            if(lines>20){
                k_puts("\npress any key to continue...");
                k_getch_extended();
                lines=1;
            }
            Screen::setfgcolor(Screen::LIGHT_CYAN);
            k_puts("\nEDX_7_0");
            Screen::setfgcolor(Screen::WHITE);
            lines++;
            for(uint32_t i=0;i<32;i++){
                if(i==0||i==1||i==5||i==6||i==7||i==11||i==12||i==17||i==21||i==23)continue;//skip reserved bits
                if(edx7_0&(1U<<i)){
                    if(lines>=24){
                        k_puts("\npress any key to continue...");
                        k_getch_extended();
                        lines=1;
                    }
                    lines++;
                    k_putc('\n');
                    k_puts(features_edx_7_0_str[i]);
                }
            }
        }
        if(eax7_1&(1U<<5)){
            Screen::setfgcolor(Screen::LIGHT_CYAN);
            k_puts("\nEAX_7_1");
            Screen::setfgcolor(Screen::WHITE);
            if(lines>=24){
                k_puts("\npress any key to continue...");
                k_getch_extended();
                lines=1;
            }
            lines++;
            k_putc('\n');
            k_puts(features_eax_7_1_str[5]);
        }
    }
}
extern "C" uint32_t has_cpuid_impl();

void CPUID::init(){
    b_has_cpuid=has_cpuid_impl();
    //clear vars
    eax0=0;
    ebx0=0;
    ecx0=0;
    edx0=0;
    eax1=0;
    ebx1=0;
    ecx1=0;
    edx1=0;
    eax7_0=0;
    ebx7_0=0;
    ecx7_0=0;
    edx7_0=0;
    eax7_1=0;
    ebx7_1=0;
    ecx7_1=0;
    edx7_1=0;
    if(b_has_cpuid){
        asm volatile(
            R"asm(
                pusha //save registers
                push %0
                push %1
                push %2
                push %3
                call cpuid_0
                popa //restore registers
            )asm"
            :
            :
            "r"(&eax0),
            "r"(&ebx0),
            "r"(&ecx0),
            "r"(&edx0)
        );
        asm volatile(
            R"asm(
                pusha //save registers
                push %0
                push %1
                push %2
                push %3
                call cpuid_1
                popa //restore registers
            )asm"
            :
            :
            "r"(&eax1),
            "r"(&ebx1),
            "r"(&ecx1),
            "r"(&edx1)
        );
        if(eax0>=7){
            asm volatile(
                R"asm(
                    pusha //save registers
                    push %0
                    push %1
                    push %2
                    push %3
                    call cpuid_7_0
                    popa //restore registers
                )asm"
                :
                :
                "r"(&eax7_0),
                "r"(&ebx7_0),
                "r"(&ecx7_0),
                "r"(&edx7_0)
            );
            asm volatile(
                R"asm(
                    pusha //save registers
                    push %0
                    push %1
                    push %2
                    push %3
                    call cpuid_7_1
                    popa //restore registers
                )asm"
                :
                :
                "r"(&eax7_1),
                "r"(&ebx7_1),
                "r"(&ecx7_1),
                "r"(&edx7_1)
            );
        }
    }
}

bool CPUID::has_cpuid(){
    return b_has_cpuid;
}

void CPUID::get(uint32_t &c1,uint32_t &d1){
    c1=ecx1;
    d1=edx1;
}

void CPUID::get(uint32_t &c1,uint32_t &d1,uint32_t &b1,uint32_t &c2,uint32_t &d2,uint32_t &a1){
    a1=eax7_1;
    c1=ecx1;
    d1=edx1;
    b1=ebx7_0;
    c2=ecx7_0;
    d2=edx7_0;
}

void CPUID::cmd_cpuid(){
    print_features();
}

bool CPUID::has(uint32_t c1,uint32_t d1){
    return ((c1&ecx1)==c1)&&((d1&edx1)==d1);
}

bool CPUID::has(uint32_t c1,uint32_t d1,uint32_t b1,uint32_t c2,uint32_t d2,uint32_t a1){
    return ((c1&ecx1)==c1)&&((d1&edx1)==d1)&&((b1&ebx7_0)==b1)&&((c2&ecx7_0)==c2)&&((d2&edx7_0)==d2)&&((a1&eax7_1)==a1);
    
}

void CPUID::check(){
    uint32_t ecx,edx;
    get(ecx,edx);
    if(!(edx&FEAT_EDX_1_MSR)){
        k_abort_s("MSR not supported");
    }
}
