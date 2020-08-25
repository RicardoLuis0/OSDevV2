#ifndef X86_CR_H_INCLUDED
#define X86_CR_H_INCLUDED

#include <stdint.h>

namespace CR0{
    enum flags {
        PE = 1U <<  0,//protected mode enable bit
        MP = 1U <<  1,//monitor co-processor
        EM = 1U <<  2,//x87 is present
        TS = 1U <<  3,//x87 task switched
        ET = 1U <<  4,//x87 type
        NE = 1U <<  5,//x87 numeric error
        //bits 6-15 reserved
        WP = 1U << 16,//ring 0 write protect
        //bit 17 reserved
        AM = 1U << 18,//ring 3 alignment check
        //bits 19-28 reserved
        NW = 1U << 29,//write-through caching disable bit
        CD = 1U << 30,//memory caching disable bit
        PG = 1U << 31,//paging enable bit
    };
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

namespace CR4 {
    
    enum flags {
        VME        = 1U <<  0,//V8086 mode virtual interrupt support
        PVI        = 1U <<  1,//protected mode virtual interrupt support
        TSD        = 1U <<  2,//RDTSC only in ring0?
        DE         = 1U <<  3,//debugging extensions enabled
        PSE        = 1U <<  4,//is Page Size Extension (page size 4MB) enabled?
        PAE        = 1U <<  5,//is Physical Address Extension enabled?
        MCE        = 1U <<  6,//enable machine check interrupts
        PGE        = 1U <<  7,//page global enabled
        PCE        = 1U <<  8,//RDPCM at any priviledge?
        OSFXSR     = 1U <<  9,//enable FXSAVE FXRSTOR
        OSXMMEXCPT = 1U << 10,//enable unmasked SSE exceptions
        UMIP       = 1U << 11,//prevent usermode access to SGDT, SIDT, SLDT, SMSW and STR
        LA57       = 1U << 12,//enable 5-level paging
        VMXE       = 1U << 13,//VT-x, Intel Virtualization Technology
        SMXE       = 1U << 14,//Intel Trusted Execution Technology
        //bit 15 reserved
        FSGSBASE   = 1U << 16,//enable RDFSBASE RDGSBASE WRFSBASE WRGSBASE
        PCIDE      = 1U << 17,//enable PCID
        OSXAVE     = 1U << 18,//enable XSAVE
        //bit 19 reserved
        SMEP       = 1U << 20,//enable Supervisor Mode Execution Protection
        SMAP       = 1U << 21,//enable Supervisor Mode Access Prevention
        PKE        = 1U << 22,//enable Protection Key
        //bits 23-31 reserved
    };
    
    uint32_t get();
    void enableFlags(uint32_t flags);
    void disableFlags(uint32_t flags);
    bool hasFlags(uint32_t flags);
}
namespace CR{
    void cmd_crdump();
};


#endif // X86_CR_H_INCLUDED
