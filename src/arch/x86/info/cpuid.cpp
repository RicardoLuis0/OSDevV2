#include "arch/x86.h"
#include "klib.h"
#include "screen.h"

using namespace CPUID;

static const char * features_ecx_str[32] {
    "ECX_SSE3           -- SSE3",
    "ECX_PCLMUL         -- PCLMULQDQ",
    "ECX_DTES64         -- 64-bit debug store",
    "ECX_MONITOR        -- MONITOR, MWAIT (SSE3)",
    "ECX_DS_CPL         -- CPL-qualified debug store",
    "ECX_VMX            -- Virtual Machine eXtensions",
    "ECX_SMX            -- Safer Mode Extensions",
    "ECX_EST            -- Enhanced SpeedStep",
    "ECX_TM2            -- Thermal Monitor 2",
    "ECX_SSSE3          -- Supplemental SSE3",
    "ECX_CNTX_ID        -- L1 Context ID",
    "ECX_SDBG           -- Silicon Debug Interface",
    "ECX_FMA            -- Fused Multiply-Add (FMA3)",
    "ECX_CX16           -- CMPXCHG16B compare-and-swap",
    "ECX_XTPR           -- Disable task priority messages",
    "ECX_PDCM           -- Perfmon & Debug Capability",
    "reserved           -- unused",
    "ECX_PCID           -- process context identifiers (CR4 bit 17)",
    "ECX_DCA            -- Direct cahce access for DMA",
    "ECX_SSE4_1         -- SSE4.1",
    "ECX_SSE4_2         -- SSE4.2",
    "ECX_x2APIC         -- x2APIC",
    "ECX_MOVBE          -- MOVBE (big-endian)",
    "ECX_POPCNT         -- POPCNT",
    "ECX_TSC_DEADLINE   -- APIC TSC deadline",
    "ECX_AES            -- AES",
    "ECX_XSAVE          -- XSAVE,XRESTOR,XSETBV,XGETBV",
    "ECX_OSXSAVE        -- XSAVE enabled by OS",
    "ECX_AVX            -- Advanced Vector Extensions",
    "ECX_F16C           -- Half-precision float support",
    "ECX_RDRAND         -- RDRAND",
    "ECX_HYPERVISOR     -- Running under hypervisor",
};

static const char * features_edx_str[32] {
    "EDX_FPU        -- on-board FPU",
    "EDX_VME        -- virtual 8086 mode extensions",
    "EDX_DE         -- debugging extensions",
    "EDX_PSE        -- page size extension",
    "EDX_TSC        -- time stamp counter",
    "EDX_MSR        -- model-specific registers",
    "EDX_PAE        -- physical address extension",
    "EDX_MCE        -- machine check exception",
    "EDX_CX8        -- CMPXCHG8 compare-and-swap",
    "EDX_APIC       -- on-board APIC",
    "reserved       -- unused",
    "EDX_SEP        -- SYSENTER & SYSEXIT",
    "EDX_MTRR       -- Memory type range registers",
    "EDX_PGE        -- Page global enable bit (in CR4)",
    "EDX_MCA        -- Machine check architecture",
    "EDX_CMOV       -- Conditional move",
    "EDX_PAT        -- Page attribute table",
    "EDX_PSE_36     -- 36-bit page extension",
    "EDX_PSN        -- Processor serial number",
    "EDX_CLFSH      -- CLFLUSH (SSE2)",
    "reserved       -- unused",
    "EDX_DTES       -- Debug store",
    "EDX_ACPI       -- ACPI thermal control MSRs",
    "EDX_MMX        -- MMX",
    "EDX_FXSR       -- FXSAVE,FXRESTOR, CR4 bit 9",
    "EDX_SSE        -- SSE",
    "EDX_SSE2       -- SSE2",
    "EDX_SS         -- CPU cache self-snoop",
    "EDX_HTT        -- Hyper-threading",
    "EDX_TM1        -- Thermal Monitor 1",
    "EDX_IA64       -- Running as x86 emulation on 64-bit CPU",
    "EDX_PBE        -- Pending Break Enable wakeup capability",
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

void CPUID::get(uint32_t &ecx,uint32_t &edx){
    ecx=0;
    edx=0;
    asm volatile(
        R"asm(
            pusha //save registers just in case
            push %0
            push %1
            call cpuid_features
            popa //restore registers
        )asm"
        :
        :
        "r"(&ecx),
        "r"(&edx)
    );
}

void CPUID::cmd_cpuid(){
    uint32_t ecx,edx;
    get(ecx,edx);
    print_features(ecx,edx);
}

bool CPUID::has(uint32_t ecx,uint32_t edx){
    uint32_t ecx2,edx2;
    get(ecx2,edx2);
    return ((ecx&ecx2)==ecx)&&((edx&edx2)==edx);
}

void CPUID::check(){
    uint32_t ecx,edx;
    get(ecx,edx);
    if(!(edx&FEAT_EDX_MSR)){
        k_abort_s("MSR not supported");
    }
    if(!(edx&FEAT_EDX_APIC)){
        k_abort_s("APIC not supported");
    }
}
