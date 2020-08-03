#ifndef X86_CR_H_INCLUDED
#define X86_CR_H_INCLUDED

#include <stdint.h>

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


#endif // X86_CR_H_INCLUDED
