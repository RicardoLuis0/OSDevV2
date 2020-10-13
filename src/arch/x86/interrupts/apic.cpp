#include "arch/x86.h"
#include "klib.h"
#include "screen.h"

enum lapic_regs : uint32_t {
    LAPIC_ID =            0x20U, // read-writeU, Local APIC ID Register
    LAPIC_VER =           0x30U, // read-onlyU, Local APIC Version Register
    LAPIC_TPR =           0x80U, // read-writeU, Task Priority Register
    LAPIC_APR =           0x90U, // read-onlyU, Arbitration Priority Register
    LAPIC_PPR =           0xA0U, // read-onlyU, Processor Priority Register
    LAPIC_EOI =           0xB0U, // write-onlyU, EOI Register
    LAPIC_RRD =           0xC0U, // read-onlyU, Remote Read Register
    LAPIC_LDR =           0xD0U, // read-writeU, Local Destination Register
    LAPIC_DFR =           0xE0U, // read-writeU, Destination Format Register
    LAPIC_SIV =           0xF0U, // read-writeU, Spurious Interrupt Vector Register
    LAPIC_ISR_0 =        0x100U, // read-onlyU, In-Service Register bits 31:0
    LAPIC_ISR_1 =        0x110U, // read-onlyU, In-Service Register bits 63:32
    LAPIC_ISR_2 =        0x120U, // read-onlyU, In-Service Register bits 95:64
    LAPIC_ISR_3 =        0x130U, // read-onlyU, In-Service Register bits 127:96
    LAPIC_ISR_4 =        0x140U, // read-onlyU, In-Service Register bits 159:128
    LAPIC_ISR_5 =        0x150U, // read-onlyU, In-Service Register bits 191:160
    LAPIC_ISR_6 =        0x160U, // read-onlyU, In-Service Register bits 223:192
    LAPIC_ISR_7 =        0x170U, // read-onlyU, In-Service Register bits 255:224
    LAPIC_TMR_0 =        0x180U, // read-onlyU, Trigger Mode Register bits 31:0
    LAPIC_TMR_1 =        0x190U, // read-onlyU, Trigger Mode Register bits 63:32
    LAPIC_TMR_2 =        0x1A0U, // read-onlyU, Trigger Mode Register bits 95:64
    LAPIC_TMR_3 =        0x1B0U, // read-onlyU, Trigger Mode Register bits 127:96
    LAPIC_TMR_4 =        0x1C0U, // read-onlyU, Trigger Mode Register bits 159:128
    LAPIC_TMR_5 =        0x1D0U, // read-onlyU, Trigger Mode Register bits 191:160
    LAPIC_TMR_6 =        0x1E0U, // read-onlyU, Trigger Mode Register bits 223:192
    LAPIC_TMR_7 =        0x1F0U, // read-onlyU, Trigger Mode Register bits 255:224
    LAPIC_IRR_0 =        0x200U, // read-onlyU, Interrupt Request Register bits 31:0
    LAPIC_IRR_1 =        0x210U, // read-onlyU, Interrupt Request Register bits 63:32
    LAPIC_IRR_2 =        0x220U, // read-onlyU, Interrupt Request Register bits 95:64
    LAPIC_IRR_3 =        0x230U, // read-onlyU, Interrupt Request Register bits 127:96
    LAPIC_IRR_4 =        0x240U, // read-onlyU, Interrupt Request Register bits 159:128
    LAPIC_IRR_5 =        0x250U, // read-onlyU, Interrupt Request Register bits 191:160
    LAPIC_IRR_6 =        0x260U, // read-onlyU, Interrupt Request Register bits 223:192
    LAPIC_IRR_7 =        0x270U, // read-onlyU, Interrupt Request Register bits 255:224
    LAPIC_ESR =          0x280U, // read-onlyU, Error Status Register
    LAPIC_LVT_CMCI =     0x2F0U, // read-writeU, LVT CMCI Register
    LAPIC_ICR_0 =        0x300U, // read-writeU, Interrupt Command Register bits 31:0
    LAPIC_ICR_1 =        0x310U, // read-writeU, Interrupt Command Register bits 63:32
    LAPIC_LVT_TR =       0x320U, // read-writeU, LVT Timer Register
    LAPIC_LVT_TSR =      0x330U, // read-writeU, LVT Thermal Sensor Register
    LAPIC_LVT_PMCR =     0x340U, // read-writeU, LVT Performance Monitoring Counters Register
    LAPIC_LVT_LINT0 =    0x350U, // read-writeU, LVT LINT0 Register
    LAPIC_LVT_LINT1 =    0x360U, // read-writeU, LVT LINT0 Register
    LAPIC_LVT_ERR =      0x370U, // read-writeU, LVT Error Register
    LAPIC_TIMER_ICR =    0x380U, // read-writeU, Timer Initial Count Register
    LAPIC_TIMER_CCR =    0x390U, // read-onlyU, Timer Current Count Register
    LAPIC_TIMER_DCR =    0x39EU, // read-writeU, Timer Divide Configuration Register
};

enum ioapic_regs : uint32_t{
    IOAPIC_ID  = 0x0U,//bits 24-27 are ID
    IOAPIC_VER = 0x1U,//bits 0-8 are version, bits 16-23 are max number of IRQs
    IOAPIC_ARB = 0x2U,
};

#define LAPIC_MSR 0x1B
#define LAPIC_MSR_ENABLE_BIT (1U << 11)

#define IOAPIC_SELECT 0x0U
#define IOAPIC_DATA 0x10U

#define LAPIC_MSR_X2APIC_ENABLE_BIT (1U << 10)

#define REG2MSR(reg) ((reg>>4)+0x800)//ex. 0x20 -> 0x802

using namespace ACPI;

extern MADT::Table * madt;

namespace APIC {
    
    static bool x2apic;
    
    static uintptr_t lapic_base(){
        uint64_t msr;
        MSR::get(LAPIC_MSR,&msr);
        return msr&0xFFFFF000;//mask off lower bits
    }
    
    [[maybe_unused]]
    static inline uint32_t lapic_register_get(uint32_t reg){
        if(x2apic){
            if(reg==LAPIC_ICR_0){
                uint32_t lo,hi;
                MSR::get(0x830,&lo,&hi);
                return lo;
            }else if(reg==LAPIC_ICR_1){
                uint32_t lo,hi;
                MSR::get(0x830,&lo,&hi);
                return hi;
            }else{
                uint32_t lo,hi;
                MSR::get(REG2MSR(reg),&lo,&hi);
                return lo;
            }
        }else{
            return MMIO32(lapic_base(),reg);
        }
    }
    
    [[maybe_unused]]
    static inline void lapic_register_set(uint32_t reg,uint32_t val){
        if(x2apic){
            if(reg==LAPIC_ICR_0){
                uint32_t lo,hi;
                MSR::get(0x830,&lo,&hi);
                MSR::set(0x830,val,hi);
            }else if(reg==LAPIC_ICR_1){
                uint32_t lo,hi;
                MSR::get(0x830,&lo,&hi);
                MSR::set(0x830,lo,val);
            }else{
                MSR::set(REG2MSR(reg),val,0);
            }
        }else{
            MMIO32(lapic_base(),reg)=val;
        }
    }
    
    [[maybe_unused]]
    static inline void lapic_register_and(uint32_t reg,uint32_t val){
        if(x2apic){
            if(reg==LAPIC_ICR_0){
                uint32_t lo,hi;
                MSR::get(0x830,&lo,&hi);
                MSR::set(0x830,val&lo,hi);
            }else if(reg==LAPIC_ICR_1){
                uint32_t lo,hi;
                MSR::get(0x830,&lo,&hi);
                MSR::set(0x830,lo,val&hi);
            }else{
                uint32_t lo,hi;
                MSR::get(REG2MSR(reg),&lo,&hi);
                MSR::set(REG2MSR(reg),val&lo,0);
            }
        }else{
            uintptr_t base=lapic_base();
            MMIO32(base,reg)=MMIO32(base,reg)&val;
        }
    }
    
    [[maybe_unused]]
    static inline void lapic_register_or(uint32_t reg,uint32_t val){
        if(x2apic){
            if(reg==LAPIC_ICR_0){
                uint32_t lo,hi;
                MSR::get(0x830,&lo,&hi);
                MSR::set(0x830,val|lo,hi);
            }else if(reg==LAPIC_ICR_1){
                uint32_t lo,hi;
                MSR::get(0x830,&lo,&hi);
                MSR::set(0x830,lo,val|hi);
            }else{
                uint32_t lo,hi;
                MSR::get(REG2MSR(reg),&lo,&hi);
                MSR::set(REG2MSR(reg),val|lo,0);
            }
        }else{
            uintptr_t base=lapic_base();
            MMIO32(base,reg)=MMIO32(base,reg)|val;
        }
    }
    
    [[maybe_unused]]
    static uint32_t ioapic_get(uintptr_t base,uint32_t reg){
        MMIO32(base,IOAPIC_SELECT)=reg;
        return MMIO32(base,IOAPIC_DATA);
    }
    
    [[maybe_unused]]
    static void ioapic_set(uintptr_t base,uint32_t reg,uint32_t val){
        MMIO32(base,IOAPIC_SELECT)=reg;
        MMIO32(base,IOAPIC_DATA)=val;
    }
    
    [[maybe_unused]]
    static void enable_x2apic(){
        if(!x2apic&&CPUID::has(CPUID::FEAT_ECX_1_x2APIC,0)){
            MSR::set(LAPIC_MSR,MSR::get(LAPIC_MSR)|LAPIC_MSR_X2APIC_ENABLE_BIT);
            x2apic=true;
        }
    }
    
    [[maybe_unused]]
    static void disable_x2apic(){
        if(x2apic&&(MSR::get(LAPIC_MSR)&LAPIC_MSR_X2APIC_ENABLE_BIT)){
            MSR::set(LAPIC_MSR,MSR::get(LAPIC_MSR)&~(LAPIC_MSR_ENABLE_BIT|LAPIC_MSR_X2APIC_ENABLE_BIT));//disable apic&x2apic
            MSR::set(LAPIC_MSR,MSR::get(LAPIC_MSR)|LAPIC_MSR_ENABLE_BIT);//re-enable apic without x2apic
            x2apic=false;
        }
    }
    
    static void init_x2apic(){
        if(x2apic){
            MSR::set(LAPIC_MSR,MSR::get(LAPIC_MSR)|LAPIC_MSR_X2APIC_ENABLE_BIT);
        }else if(MSR::get(LAPIC_MSR)&LAPIC_MSR_X2APIC_ENABLE_BIT){
            MSR::set(LAPIC_MSR,MSR::get(LAPIC_MSR)&~(LAPIC_MSR_ENABLE_BIT|LAPIC_MSR_X2APIC_ENABLE_BIT));//disable apic&x2apic
            MSR::set(LAPIC_MSR,MSR::get(LAPIC_MSR)|LAPIC_MSR_ENABLE_BIT);//re-enable apic without x2apic
        }
    }
    
    [[maybe_unused]]
    static void apic_info(){
        Screen::write_s("\n\nLAPIC BASE:");
        Screen::write_h(lapic_base());
        Screen::write_s("\n\n");
        Screen::write_s("\n\nLAPIC ID:");
        Screen::write_h(lapic_register_get(LAPIC_ID));
        Screen::write_s("\n\n");
        Screen::write_s("\n\nLAPIC ENABLED:");
        Screen::write_s((MSR::get(LAPIC_MSR)&LAPIC_MSR_ENABLE_BIT)?"yes":"no");
        Screen::write_s("\n\n");
        Screen::write_s("\n\nLAPIC SIV:");
        Screen::write_h(lapic_register_get(LAPIC_SIV));
        Screen::write_s("\n\n");
    }
    
    static void init_lapic(){
        //enable LAPIC in case it isn't already enabled
        MSR::set(LAPIC_MSR,MSR::get(LAPIC_MSR)|LAPIC_MSR_ENABLE_BIT);
        
        init_x2apic();
        
        //enable LAPIC's SIV (spurious interrupt vector) in case it isn't already enabled
        lapic_register_or(LAPIC_SIV,0x1FF);
    }
    
    struct ioapic_info {
        uintptr_t base;
        uint8_t id;
        uint8_t version;
        uint8_t max_irq;
        //TODO
    };
    
    ioapic_info * ioapics;
    uint32_t ioapic_count;
    
    [[maybe_unused]]
    static uintptr_t ioapic_base(uint32_t id){
        for(uint32_t i=0;i<ioapic_count;i++){
            if(ioapics[i].id==id){
                return ioapics[i].base;
            }
        }
        k_abort_fmt("can't find base address for non-existent IOAPIC #%d",id);
    }
    
    void init(){
        Screen::write_s("\n -Enabling APIC...");
        
        if(madt->flags&MADT::LEGACY_PIC){//mask out legacy PIC if necessary
            PIC::reset(0x20,0x28);
        }
        
        x2apic=CPUID::has(CPUID::FEAT_ECX_1_x2APIC,0);
        
        init_lapic();//initialize LAPIC
        
        ioapic_count=0;
        for(uint32_t i=0;i<MADT::entry_count;i++){//NOTE maybe build an array of IOAPIC addresses instead of going through whole MADT table?
                
            if(MADT::entries[i]->type==MADT::Entry::IO_APIC){
                ioapic_count++;
            }
        }
        
        ioapics=reinterpret_cast<ioapic_info *>(Memory::alloc_virt_page(Memory::pages_to_fit(sizeof(ioapic_info)*ioapic_count)));
        
        for(uint32_t i=0,n=0;i<MADT::entry_count;i++){//NOTE maybe build an array of IOAPIC addresses instead of going through whole MADT table?
            if(MADT::entries[i]->type==MADT::Entry::IO_APIC){
                MADT::IOAPICEntry * entry=reinterpret_cast<MADT::IOAPICEntry*>(MADT::entries[i]);
                
                uint32_t id=(ioapic_get(entry->address,IOAPIC_ID)>>24)&0xF;
                
                //don't double check ID, since virtualbox gives wrong id in the MADT
                /*
                if(id!=entry->id){
                    k_abort_fmt("invalid ID for IOAPIC #%u, expected %x got %x",n,entry->id,id);
                }
                */
                
                auto &info=ioapics[n];
                info.base=entry->address;
                info.id=id;
                uint32_t ver=ioapic_get(entry->address,IOAPIC_VER);
                info.version=ver&0xFF;
                info.max_irq=((ver>>16)&0xFF)+1;
                #ifdef IOAPIC_DEBUG
                    Screen::write_s("\n  .IOAPIC #");
                    Screen::write_i(n);
                    Screen::write_s(" detected\n   +base=");
                    Screen::write_h(info.base);
                    Screen::write_s("\n   +id=");
                    Screen::write_i(info.id);
                    Screen::write_s(" (MADT says ");
                    Screen::write_i(entry->id);
                    Screen::write_s(")\n   +version=");
                    Screen::write_i(info.version);
                    Screen::write_s("\n   +max_irq=");
                    Screen::write_i(info.max_irq);
                #endif // IOAPIC_DEBUG
                n++;
            }
        }
        
        Screen::write_s("\n  .Found ");
        Screen::write_i(ioapic_count);
        Screen::write_s(" IOAPICs...");
        
        
        //TODO initialize IOAPIC
        k_abort_s("IOAPIC unimplemented");
    }
    
    bool supported(){
        //return CPUID::has(0,CPUID::FEAT_EDX_1_APIC);
        return false;
    }
    
    void enable(uint8_t irq){
        k_abort_s("APIC::enable unimplemented");
    }
    
    void disable(uint8_t irq){
        k_abort_s("APIC::disable unimplemented");
    }
    
    uint8_t get_mapping(uint8_t irq){
        k_abort_s("APIC::get_mapping unimplemented");
    }
    
}

