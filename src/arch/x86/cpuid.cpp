#include "arch/x86.h"
#include "klib.h"
#include "screen.h"

using namespace CPUID;

static const char * cpuid_features_ecx_str[32] {
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

static const char * cpuid_features_edx_str[32] {
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
    "CPUID_FEAT_EDX_IA64 -- Running as x86 emulation on 64-bit CPU",
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

void CPUID::cpuid_get(uint32_t &ecx,uint32_t &edx){
    ecx=0;
    edx=0;
    asm volatile(
        R"asm(
            push %%ecx //save modified registers just in case
            push %%edx
            push %0
            push %1
            call cpuid_features
            pop %%edx //restore modified registers
            pop %%ecx
        )asm"
        :
        :
        "r"(&ecx),
        "r"(&edx)
    );
}

void CPUID::cmd_cpuid(){
    uint32_t ecx,edx;
    cpuid_get(ecx,edx);
    print_features(ecx,edx);
}

bool CPUID::cpuid_has(uint32_t ecx,uint32_t edx){
    uint32_t ecx2,edx2;
    cpuid_get(ecx2,edx2);
    return ((ecx&ecx2)==ecx)&&((edx&edx2)==edx);
}

void CPUID::check(){
    uint32_t ecx,edx;
    cpuid_get(ecx,edx);
    if(!(edx&CPUID_FEAT_EDX_MSR)){
        k_abort_s("MSR not supported");
    }
    if(!(edx&CPUID_FEAT_EDX_APIC)){
        k_abort_s("APIC not supported");
    }
}
