#ifndef X86_H_INCLUDED
#define X86_H_INCLUDED

#include "stdc/stdint.h"
#include "stdc/stddef.h"
#include "extern/multiboot.h"
#include <stdatomic.h>

extern "C" uint32_t has_cpuid();

namespace CPU {
    struct cpu_state{
        //TODO implement
    };
    void save(cpu_state*);
    void restore(cpu_state*);
}

namespace CR{
    void cmd_crdump();
    enum CR0_flags {
        CR0_PE = 1U <<  0,//protected mode enable bit
        CR0_MP = 1U <<  1,//monitor co-processor
        CR0_EM = 1U <<  2,//x87 is present
        CR0_TS = 1U <<  3,//x87 task switched
        CR0_ET = 1U <<  4,//x87 type
        CR0_NE = 1U <<  5,//x87 numeric error
        //bits 6-15 reserved
        CR0_WP = 1U << 16,//ring 0 write protect
        //bit 17 reserved
        CR0_AM = 1U << 18,//ring 3 alignment check
        //bits 19-28 reserved
        CR0_NW = 1U << 29,//write-through caching disable bit
        CR0_CD = 1U << 30,//memory caching disable bit
        CR0_PG = 1U << 31,//paging enable bit
    };
    namespace CR0{
        uint32_t get();
        void enableFlags(uint32_t flags);
        void disableFlags(uint32_t flags);
        bool hasFlags(uint32_t flags);
    }
    namespace CR2 {
        uint32_t get();
    }
    namespace CR3 {
        uint32_t get();
        void set(uint32_t);
    }
    enum CR4_flags {
        CR4_VME        = 1U <<  0,//V8086 mode virtual interrupt support
        CR4_PVI        = 1U <<  1,//protected mode virtual interrupt support
        CR4_TSD        = 1U <<  2,//RDTSC only in ring0?
        CR4_DE         = 1U <<  3,//debugging extensions enabled
        CR4_PSE        = 1U <<  4,//is Page Size Extension (page size 4MB) enabled?
        CR4_PAE        = 1U <<  5,//is Physical Address Extension enabled?
        CR4_MCE        = 1U <<  6,//enable machine check interrupts
        CR4_PGE        = 1U <<  7,//page global enabled
        CR4_PCE        = 1U <<  8,//RDPCM at any priviledge?
        CR4_OSFXSR     = 1U <<  9,//enable FXSAVE FXRSTOR
        CR4_OSXMMEXCPT = 1U << 10,//enable unmasked SSE exceptions
        CR4_UMIP       = 1U << 11,//prevent usermode access to SGDT, SIDT, SLDT, SMSW and STR
        CR4_LA57       = 1U << 12,//enable 5-level paging
        CR4_VMXE       = 1U << 13,//VT-x, Intel Virtualization Technology
        CR4_SMXE       = 1U << 14,//Intel Trusted Execution Technology
        //bit 15 reserved
        CR4_FSGSBASE   = 1U << 16,//enable RDFSBASE RDGSBASE WRFSBASE WRGSBASE
        CR4_PCIDE      = 1U << 17,//enable PCID
        CR4_OSXAVE     = 1U << 18,//enable XSAVE
        //bit 19 reserved
        CR4_SMEP       = 1U << 20,//enable Supervisor Mode Execution Protection
        CR4_SMAP       = 1U << 21,//enable Supervisor Mode Access Prevention
        CR4_PKE        = 1U << 22,//enable Protection Key
        //bits 23-31 reserved
    };
    namespace CR4 {
        uint32_t get();
        void enableFlags(uint32_t flags);
        void disableFlags(uint32_t flags);
        bool hasFlags(uint32_t flags);
    }
};

namespace FPU {
    void init();
}

namespace CPUID {

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
        //bit 16 reserved
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
        //bit 10 reserved
        CPUID_FEAT_EDX_SEP          = 1U << 11,//SYSENTER & SYSEXIT
        CPUID_FEAT_EDX_MTRR         = 1U << 12,//Memory type range registers
        CPUID_FEAT_EDX_PGE          = 1U << 13,//Page global enable bit (in CR4)
        CPUID_FEAT_EDX_MCA          = 1U << 14,//Machine check architecture
        CPUID_FEAT_EDX_CMOV         = 1U << 15,//Conditional move
        CPUID_FEAT_EDX_PAT          = 1U << 16,//Page attribute table
        CPUID_FEAT_EDX_PSE_36       = 1U << 17,//36-bit page extension
        CPUID_FEAT_EDX_PSN          = 1U << 18,//Processor serial number
        CPUID_FEAT_EDX_CLFSH        = 1U << 19,//CLFLUSH (SSE2)
        //bit 20 reserved
        CPUID_FEAT_EDX_DTES         = 1U << 21,//Debug store
        CPUID_FEAT_EDX_ACPI         = 1U << 22,//ACPI thermal control MSRs
        CPUID_FEAT_EDX_MMX          = 1U << 23,//MMX
        CPUID_FEAT_EDX_FXSR         = 1U << 24,//FXSAVE,FXRESTOR, CR4 bit 9
        CPUID_FEAT_EDX_SSE          = 1U << 25,//SSE
        CPUID_FEAT_EDX_SSE2         = 1U << 26,//SSE2
        CPUID_FEAT_EDX_SS           = 1U << 27,//CPU cache self-snoop
        CPUID_FEAT_EDX_HTT          = 1U << 28,//Hyper-threading
        CPUID_FEAT_EDX_TM1          = 1U << 29,//Thermal Monitor 1
        CPUID_FEAT_EDX_IA64         = 1U << 30,//Running as x86 emulation on 64-bit CPU
        CPUID_FEAT_EDX_PBE          = 1U << 31,//Pending Break Enable wakeup capability
    };
    void check();
    void cpuid_get(uint32_t &ecx,uint32_t &edx);
    bool cpuid_has(uint32_t ecx,uint32_t edx);
    void cmd_cpuid();
}

namespace Memory{
    void cmd_pagefault();
    void x86_init(multiboot_info * mbd);
    constexpr uint16_t phys_page_segment=32768;
    struct physical_pages_t {
        uint32_t usage[phys_page_segment];//4GB of page usage, each page is 1 bit
        uint32_t last_usable;
    };

    uint32_t map_virtual_page(uint32_t p,uint32_t v,uint32_t n);//map 'n' contiguous pages at physicall address 'p' to virtual address 'v'

    void unmap_virtual_page(uint32_t v,uint32_t n);//unmap 'n' virtual pages at virtual address 'v'

    uint32_t get_mapping_phys(uint32_t p);//get first virtual mapping of physical address

    uint32_t get_mapping_virt(uint32_t v);//get physical mapping of virtual address

    uint32_t next_free_virt_page(uint32_t n);//get a free unused virtual address ( to pass to map_virtual_page ) that can fit 'n' pages

    void x86_paging_init();

    constexpr size_t pages_to_fit(size_t size){//calculate enough pages to fit 'size' bytes of data
        return (size%4096)?(size/4096)+1:size/4096;
    }

    namespace Internal {
        void pages_for(uint32_t addr,uint32_t len,uint32_t &p_id,uint32_t &n);
        extern physical_pages_t pages;
        bool is_phys_page_free(uint32_t page_id);//check if page with id
        void * phys_id_to_ptr(uint32_t page_id);
        void * alloc_phys_page(uint32_t n);//get 'n' new physical pages (and mark as used)
        void free_phys_page(void*,uint32_t n);//mark 'n' physical pages as unused
        void set_phys_free(uint32_t page_id_start,uint32_t page_id_end,bool new_free);
        uint32_t count_used_pages();//counts number of non-free physical pages
        uint32_t map_virtual_page_unsafe(uint32_t p,uint32_t v,uint32_t n,bool allow_remap_if_same);
        
        //map and unmap nullpointer as identity for working with raw data near it
        void map_null();
        void unmap_null();
    }
    void * alloc_virt_page(uint32_t n);//get 'n' new virtual pages (and mark as used)
    void free_virt_page(void*,uint32_t n);//mark 'n' virtual pages as unused
}

namespace Screen{
    void x86_init();
}

namespace Serial{
    void x86_init();
}

namespace GDT {
    void init();
}

namespace PIT {
    void init();
    extern volatile atomic_int_least64_t timer;
    extern const uint32_t timer_resolution;//timer resolution in ms
}

namespace IDT{
    enum gate_type {
        G_32_INT=0b1110,
        G_32_TASK=0b0101,
        G_32_TRAP=0b1111,
        //G_16_INT=0b0110,
        //G_16_TRAP=0b0111,
    };
    enum ring_type {
        RING_0=0b00,
        RING_1=0b01,
        RING_2=0b10,
        RING_3=0b11,
    };
    void init();
    void set_irq_handler(uint8_t num,void(*)(void),gate_type,ring_type);//ignores data
    void set_irq_handler(uint8_t num,void(*)(uint32_t),gate_type,ring_type);//receives data
    void set_irq_handler(uint8_t num,void(*)(uint32_t,uint32_t),gate_type,ring_type);//receives data and irq number (for multi-irq handlers)
    void set_raw_irq_handler(uint8_t irq,void * h,gate_type g,ring_type t);//h must be a pointer to an interrupt handler (that preserves register state and returns with iret)
    void set_exception_handler(uint8_t num,void(*)(void),gate_type,ring_type);//ignores data
    void set_exception_handler(uint8_t num,void(*)(uint32_t),gate_type,ring_type);//receives data
    void set_exception_handler(uint8_t num,void(*)(uint32_t,uint32_t),gate_type,ring_type);//receives data and irq number (for multi-irq handlers)
    void set_raw_exception_handler(uint8_t irq,void * h,gate_type g,ring_type t);//h must be a pointer to an interrupt handler (that preserves register state and returns with iret)
    void pic_enable(uint8_t irq);
    void pic_disable(uint8_t irq);

    template<uint32_t irq>
    void call_interrupt(uint32_t data){//call user-defined interrupt
        asm("int %1\n"
            :
            :"a"(data),
             "i"(irq));
    }
}

namespace Tasking{

    struct task_data;

    void init();
    
    void switch_task(task_data * other);
    
    uint32_t get_core();
    
    void disable_scheduler(uint32_t core);
    void enable_scheduler(uint32_t core);

    //task_data * schedule(/* ??? */);
}

#endif // X86_H_INCLUDED
