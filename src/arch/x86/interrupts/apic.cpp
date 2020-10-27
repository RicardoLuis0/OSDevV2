#include "arch/x86.h"
#include "klib.h"
#include "screen.h"
#include "serial.h"
#include "util/iter.h"

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

//source IOAPIC spec

enum ioapic_regs : uint32_t{
    IOAPIC_ID  = 0x0U,
    IOAPIC_VER = 0x1U,
    IOAPIC_ARB = 0x2U,
};

union ioapic_id {
    struct {
        uint32_t   :24;
        uint32_t id :4;
        uint32_t    :4;
    };
    uint32_t data;
};

union ioapic_ver {
    struct {
        uint32_t ver:8;//IOAPIC version
        uint32_t    :8;//reserved
        uint32_t max:8;//max valid index of redirection table
        uint32_t    :8;//reserved
    };
    uint32_t data;
};

union ioapic_arb {
    struct {
        uint32_t       :24;
        uint32_t arb_id :4;
        uint32_t        :4;
    };
    uint32_t data;
};

enum ioapic_redir_delivery_modes {
    DELIVERY_FIXED=0x0,
    DELIVERY_LOW=0x1,
    DELIVERY_SMI=0x2,
    DELIVERY_RESERVED_1=0x3,
    DELIVERY_NMI=0x4,
    DELIVERY_INIT=0x5,
    DELIVERY_RESERVED_2=0x6,
    DELIVERY_EXTINIT=0x7,
};

union ioapic_redir {
    struct {
        uint64_t interrupt_vector   : 8;//rw, interrupt to be raised on CPU
        uint64_t delivery_mode      : 3;//rw, see ioapic_redir_delivery_modes
        uint64_t destination_mode   : 1;//rw, if false, physical destination, if true, logical destination
        uint64_t delivery_status    : 1;//ro, is the irq pending?
        uint64_t pin_polarity       : 1;//rw, false active high, true active low
        uint64_t remote_irr         : 1;//ro, if level triggered, is the interrupt being processed?
        uint64_t trigger_mode       : 1;//rw, false edge triggered, true level triggered
        uint64_t mask               : 1;//rw, is IRQ masked?
        uint64_t                    :39;//reserved
        uint64_t destination        : 8;//if physical, APIC ID of target, if logical set of processors
    };
    
    uint64_t data;
    
    struct {
        uint32_t lo;
        uint32_t hi;
    };
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
        uint8_t gsi_base;
        //TODO any extra data per ioapic
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
    
    
    [[maybe_unused]]
    static ioapic_redir ioapic_get_redirection_reg(uintptr_t base,uint8_t id){
        const uint32_t reg=(id*2)+0x10;
        return {.lo=ioapic_get(base,reg),.hi=ioapic_get(base,reg+1)};
    }
    
    [[maybe_unused]]
    static void ioapic_set_redirection_reg(uintptr_t base,uint8_t id,ioapic_redir data){
        const uint32_t reg=(id*2)+0x10;
        ioapic_set(base,reg,data.lo);
        ioapic_set(base,reg+1,data.hi);
    }
    
    [[maybe_unused]]
    static bool ioapic_has_redirection(uint8_t irq){
        for(ioapic_info &ioapic:Util::CArrayIter(ioapics,ioapic_count)){
            if(irq>=ioapic.gsi_base&&irq<(ioapic.gsi_base+ioapic.max_irq)){
                return true;
            }
        }
        return false;
    }
    
    [[maybe_unused]]
    static ioapic_redir ioapic_get_redirection(uint8_t irq){
        for(ioapic_info &ioapic:Util::CArrayIter(ioapics,ioapic_count)){
            if(irq>=ioapic.gsi_base&&irq<(ioapic.gsi_base+ioapic.max_irq)){
                return ioapic_get_redirection_reg(ioapic.base,irq-ioapic.gsi_base);
            }
        }
        k_abort_s("requested IRQ not available for redirection");
    }
    
    [[maybe_unused]]
    static void ioapic_set_redirection(uint8_t irq,ioapic_redir entry){
        for(ioapic_info &ioapic:Util::CArrayIter(ioapics,ioapic_count)){
            if(irq>=ioapic.gsi_base&&irq<(ioapic.gsi_base+ioapic.max_irq)){
                ioapic_set_redirection_reg(ioapic.base,irq-ioapic.gsi_base,entry);
                return;
            }
        }
        k_abort_s("requested IRQ not available for redirection");
    }
    
    void init(){
        Screen::write_s("\n -Enabling APIC...");
        
        if(madt->flags&MADT::LEGACY_PIC){//mask out legacy PIC if necessary
            PIC::reset(0x20,0x28);
        }
        
        x2apic=CPUID::has(CPUID::FEAT_ECX_1_x2APIC,0);
        
        init_lapic();//initialize LAPIC
        
        ioapic_count=0;
        for(uint32_t i=0;i<MADT::entry_count;i++){
            if(MADT::entries[i]->type==MADT::Entry::IO_APIC){
                ioapic_count++;
            }
        }
        
        ioapics=reinterpret_cast<ioapic_info *>(Memory::alloc_virt_page(Memory::pages_to_fit(sizeof(ioapic_info)*ioapic_count)));
        
        for(uint32_t i=0,n=0;i<MADT::entry_count&&n<ioapic_count;i++){
            if(MADT::entries[i]->type==MADT::Entry::IO_APIC){
                MADT::IOAPICEntry * entry=reinterpret_cast<MADT::IOAPICEntry*>(MADT::entries[i]);
                
                ioapic_id id {.data=ioapic_get(entry->address,IOAPIC_ID)};
                
                //don't double check ID, since virtualbox gives wrong id in the MADT
                /*
                if(id!=entry->id){
                    k_abort_fmt("invalid ID for IOAPIC #%u, expected %x got %x",n,entry->id,id);
                }
                */
                
                auto &info=ioapics[n];
                info.base=entry->address;
                info.id=id.id;
                ioapic_ver ver {.data=ioapic_get(entry->address,IOAPIC_VER)};
                info.version=ver.ver;
                info.max_irq=ver.max+1;
                info.gsi_base=entry->global_system_interrupt_base;
                #ifdef IOAPIC_DEBUG
                    Serial::write_s("\n>IOAPIC #");
                    Serial::write_i(n);
                    Serial::write_s(" found\n -base = ");
                    Serial::write_h(info.base);
                    Serial::write_s("\n -id = ");
                    Serial::write_h(info.id);
                    Serial::write_s(" (MADT says ");
                    Serial::write_h(entry->id);
                    Serial::write_s(")\n -version = ");
                    Serial::write_h(info.version);
                    Serial::write_s("\n -max_irq = ");
                    Serial::write_h(info.max_irq);
                    Serial::write_s("\n -gsi_base = ");
                    Serial::write_h(info.gsi_base);
                    Serial::write_s("\n -redir:");
                    for(uint8_t j=0;j<info.max_irq;j++){
                        ioapic_redir redir=ioapic_get_redirection_reg(info.base,j);
                        Serial::write_s("\n  :irq ");
                        Serial::write_i(info.gsi_base+j);
                        Serial::write_s("\n   .vector = ");
                        Serial::write_i(redir.interrupt_vector);
                        Serial::write_s("\n   .delivery_mode = ");
                        switch(redir.delivery_mode){
                        case DELIVERY_FIXED:
                            Serial::write_s("FIXED");
                            break;
                        case DELIVERY_LOW:
                            Serial::write_s("LOW");
                            break;
                        case DELIVERY_SMI:
                            Serial::write_s("SMI");
                            break;
                        case DELIVERY_NMI:
                            Serial::write_s("NMI");
                            break;
                        case DELIVERY_INIT:
                            Serial::write_s("INIT");
                            break;
                        case DELIVERY_EXTINIT:
                            Serial::write_s("EXTINIT");
                            break;
                        default:
                        case DELIVERY_RESERVED_1:
                        case DELIVERY_RESERVED_2:
                            Serial::write_s("INVALID");
                            break;
                        }
                        Serial::write_s("\n   .destination_mode = ");
                        Serial::write_s(redir.destination_mode?"logical":"physical");
                        Serial::write_s("\n   .pin_polarity = ");
                        Serial::write_s(redir.pin_polarity?"low":"high");
                        Serial::write_s("\n   .trigger_mode = ");
                        Serial::write_s(redir.trigger_mode?"level":"edge");
                        Serial::write_s("\n   .mask = ");
                        Serial::write_s(redir.trigger_mode?"true":"false");
                        Serial::write_s("\n   .destination = ");
                        Serial::write_h(redir.destination);
                        Serial::write_s("\n   .raw_data = ");
                        Serial::write_h(redir.data);
                    }
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
        if(ioapic_has_redirection(irq)){
            return ioapic_get_redirection(irq).interrupt_vector;
        }else{
            return irq;
        }
    }
    
    uint8_t remap(uint8_t from,uint8_t to){
        if(ioapic_has_redirection(from)){
            k_abort_s("APIC::remap unimplemented");
        }else{
            return from;
        }
    }
    
}

