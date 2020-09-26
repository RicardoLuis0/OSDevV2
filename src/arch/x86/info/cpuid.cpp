#include "arch/x86.h"
#include "klib.h"
#include "screen.h"

using namespace CPUID;

static const char * features_ecx_str[32] {
    "ECX_1_SSE3           -- SSE3",
    "ECX_1_PCLMUL         -- PCLMULQDQ",
    "ECX_1_DTES64         -- 64-bit debug store",
    "ECX_1_MONITOR        -- MONITOR, MWAIT (SSE3)",
    "ECX_1_DS_CPL         -- CPL-qualified debug store",
    "ECX_1_VMX            -- Virtual Machine eXtensions",
    "ECX_1_SMX            -- Safer Mode Extensions",
    "ECX_1_EST            -- Enhanced SpeedStep",
    "ECX_1_TM2            -- Thermal Monitor 2",
    "ECX_1_SSSE3          -- Supplemental SSE3",
    "ECX_1_CNTX_ID        -- L1 Context ID",
    "ECX_1_SDBG           -- Silicon Debug Interface",
    "ECX_1_FMA            -- Fused Multiply-Add (FMA3)",
    "ECX_1_CX16           -- CMPXCHG16B compare-and-swap",
    "ECX_1_XTPR           -- Disable task priority messages",
    "ECX_1_PDCM           -- Perfmon & Debug Capability",
    "reserved           -- unused",
    "ECX_1_PCID           -- process context identifiers (CR4 bit 17)",
    "ECX_1_DCA            -- Direct cahce access for DMA",
    "ECX_1_SSE4_1         -- SSE4.1",
    "ECX_1_SSE4_2         -- SSE4.2",
    "ECX_1_x2APIC         -- x2APIC",
    "ECX_1_MOVBE          -- MOVBE (big-endian)",
    "ECX_1_POPCNT         -- POPCNT",
    "ECX_1_TSC_DEADLINE   -- APIC TSC deadline",
    "ECX_1_AES            -- AES",
    "ECX_1_XSAVE          -- XSAVE,XRESTOR,XSETBV,XGETBV",
    "ECX_1_OSXSAVE        -- XSAVE enabled by OS",
    "ECX_1_AVX            -- Advanced Vector Extensions",
    "ECX_1_F16C           -- Half-precision float support",
    "ECX_1_RDRAND         -- RDRAND",
    "ECX_1_HYPERVISOR     -- Running under hypervisor",
};

static const char * features_edx_str[32] {
    "EDX_1_FPU        -- on-board FPU",
    "EDX_1_VME        -- virtual 8086 mode extensions",
    "EDX_1_DE         -- debugging extensions",
    "EDX_1_PSE        -- page size extension",
    "EDX_1_TSC        -- time stamp counter",
    "EDX_1_MSR        -- model-specific registers",
    "EDX_1_PAE        -- physical address extension",
    "EDX_1_MCE        -- machine check exception",
    "EDX_1_CX8        -- CMPXCHG8 compare-and-swap",
    "EDX_1_APIC       -- on-board APIC",
    "reserved       -- unused",
    "EDX_1_SEP        -- SYSENTER & SYSEXIT",
    "EDX_1_MTRR       -- Memory type range registers",
    "EDX_1_PGE        -- Page global enable bit (in CR4)",
    "EDX_1_MCA        -- Machine check architecture",
    "EDX_1_CMOV       -- Conditional move",
    "EDX_1_PAT        -- Page attribute table",
    "EDX_1_PSE_36     -- 36-bit page extension",
    "EDX_1_PSN        -- Processor serial number",
    "EDX_1_CLFSH      -- CLFLUSH (SSE2)",
    "reserved       -- unused",
    "EDX_1_DTES       -- Debug store",
    "EDX_1_ACPI       -- ACPI thermal control MSRs",
    "EDX_1_MMX        -- MMX",
    "EDX_1_FXSR       -- FXSAVE,FXRESTOR, CR4 bit 9",
    "EDX_1_SSE        -- SSE",
    "EDX_1_SSE2       -- SSE2",
    "EDX_1_SS         -- CPU cache self-snoop",
    "EDX_1_HTT        -- Hyper-threading",
    "EDX_1_TM1        -- Thermal Monitor 1",
    "EDX_1_IA64       -- Running as x86 emulation on 64-bit CPU",
    "EDX_1_PBE        -- Pending Break Enable wakeup capability",
};

void print_features(uint32_t ecx,uint32_t edx){
    uint32_t lines=0;
    Screen::setfgcolor(Screen::LIGHT_CYAN);
    k_puts("\nECX");
    Screen::setfgcolor(Screen::WHITE);
    lines++;
    for(uint32_t i=0;i<32;i++){
        if(ecx&(1<<i)){
            if(lines==24){
                k_puts("\npress any key to continue...");
                k_getch_extended();
                lines=0;
            }
            lines++;
            k_putc('\n');
            k_puts(features_ecx_str[i]);
        }
    }
    if(lines>20){
        k_puts("\npress any key to continue...");
        k_getch_extended();
        lines=0;
    }
    Screen::setfgcolor(Screen::LIGHT_CYAN);
    k_puts("\nEDX");
    Screen::setfgcolor(Screen::WHITE);
    lines++;
    for(uint32_t i=0;i<32;i++){
        if(edx&(1<<i)){
            if(lines==24){
                k_puts("\npress any key to continue...");
                k_getch_extended();
                lines=0;
            }
            lines++;
            k_putc('\n');
            k_puts(features_edx_str[i]);
        }
    }
}

extern "C" uint32_t has_cpuid_impl();

namespace CPUID {
    bool b_has_cpuid;
    uint32_t ecx1,edx1;
}

void CPUID::init(){
    b_has_cpuid=has_cpuid_impl();
    ecx1=0;
    edx1=0;
    if(b_has_cpuid){
        asm volatile(
            R"asm(
                pusha //save registers
                push %0
                push %1
                call cpuid_features
                popa //restore registers
            )asm"
            :
            :
            "r"(&ecx1),
            "r"(&edx1)
        );
    }
}

bool CPUID::has_cpuid(){
    return b_has_cpuid;
}

void CPUID::get(uint32_t &c,uint32_t &d){
    c=ecx1;
    d=edx1;
}

void CPUID::cmd_cpuid(){
    print_features(ecx1,edx1);
}

bool CPUID::has(uint32_t c1,uint32_t d1){
    return ((c1&ecx1)==c1)&&((d1&edx1)==d1);
}

void CPUID::check(){
    uint32_t ecx,edx;
    get(ecx,edx);
    if(!(edx&FEAT_EDX_1_MSR)){
        k_abort_s("MSR not supported");
    }
    if(!(edx&FEAT_EDX_1_APIC)){
        k_abort_s("APIC not supported");
    }
}
