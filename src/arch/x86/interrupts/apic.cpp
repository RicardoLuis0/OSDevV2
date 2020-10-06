#include "arch/x86.h"
#include "klib.h"
#include "screen.h"

#define LAPIC_ID 0x20//read-write, Local APIC ID Register
#define LAPIC_VER 0x30 //read-only, Local APIC Version Register
#define LAPIC_TPR 0x80 //read-write, Task Priority Register
#define LAPIC_APR 0x90 //read-only, Arbitration Priority Register
#define LAPIC_PPR 0xA0 //read-only, Processor Priority Register
#define LAPIC_EOI 0xB0 //write-only, EOI Register
#define LAPIC_RRD 0xC0 //read-only, Remote Read Register
#define LAPIC_LDR 0xD0 //read-write, Local Destination Register
#define LAPIC_DFR 0xE0 //read-write, Destination Format Register
#define LAPIC_SIV 0xF0 //read-write, Spurious Interrupt Vector Register
#define LAPIC_ISR_0 0x100 //read-only, In-Service Register bits 31:0
#define LAPIC_ISR_1 0x110 //read-only, In-Service Register bits 63:32
#define LAPIC_ISR_2 0x120 //read-only, In-Service Register bits 95:64
#define LAPIC_ISR_3 0x130 //read-only, In-Service Register bits 127:96
#define LAPIC_ISR_4 0x140 //read-only, In-Service Register bits 159:128
#define LAPIC_ISR_5 0x150 //read-only, In-Service Register bits 191:160
#define LAPIC_ISR_6 0x160 //read-only, In-Service Register bits 223:192
#define LAPIC_ISR_7 0x170 //read-only, In-Service Register bits 255:224
#define LAPIC_TMR_0 0x180 //read-only, Trigger Mode Register bits 31:0
#define LAPIC_TMR_1 0x190 //read-only, Trigger Mode Register bits 63:32
#define LAPIC_TMR_2 0x1A0 //read-only, Trigger Mode Register bits 95:64
#define LAPIC_TMR_3 0x1B0 //read-only, Trigger Mode Register bits 127:96
#define LAPIC_TMR_4 0x1C0 //read-only, Trigger Mode Register bits 159:128
#define LAPIC_TMR_5 0x1D0 //read-only, Trigger Mode Register bits 191:160
#define LAPIC_TMR_6 0x1E0 //read-only, Trigger Mode Register bits 223:192
#define LAPIC_TMR_7 0x1F0 //read-only, Trigger Mode Register bits 255:224
#define LAPIC_IRR_0 0x200 //read-only, Interrupt Request Register bits 31:0
#define LAPIC_IRR_1 0x210 //read-only, Interrupt Request Register bits 63:32
#define LAPIC_IRR_2 0x220 //read-only, Interrupt Request Register bits 95:64
#define LAPIC_IRR_3 0x230 //read-only, Interrupt Request Register bits 127:96
#define LAPIC_IRR_4 0x240 //read-only, Interrupt Request Register bits 159:128
#define LAPIC_IRR_5 0x250 //read-only, Interrupt Request Register bits 191:160
#define LAPIC_IRR_6 0x260 //read-only, Interrupt Request Register bits 223:192
#define LAPIC_IRR_7 0x270 //read-only, Interrupt Request Register bits 255:224
#define LAPIC_ESR 0x280 //read-only, Error Status Register
#define LAPIC_LVT_CMCI 0x2F0 //read-write, LVT CMCI Register
#define LAPIC_ICR_0 0x300 //read-write, Interrupt Command Register bits 31:0
#define LAPIC_ICR_1 0x310 //read-write, Interrupt Command Register bits 63:32
#define LAPIC_LVT_TR 0x320 //read-write, LVT Timer Register
#define LAPIC_LVT_TSR 0x330 //read-write, LVT Thermal Sensor Register
#define LAPIC_LVT_PMCR 0x340 //read-write, LVT Performance Monitoring Counters Register
#define LAPIC_LVT_LINT0 0x350 //read-write, LVT LINT0 Register
#define LAPIC_LVT_LINT1 0x360 //read-write, LVT LINT0 Register
#define LAPIC_LVT_ERR 0x370 //read-write, LVT Error Register
#define LAPIC_TIMER_ICR 0x380 //read-write, Timer Initial Count Register
#define LAPIC_TIMER_CCR 0x390 //read-only, Timer Current Count Register
#define LAPIC_TIMER_DCR 0x39E //read-write, Timer Divide Configuration Register

#define LAPIC_MSR 0x1B
#define LAPIC_MSR_ENABLE_BIT (1U << 11)

#define LAPIC_MSR_X2APIC_ENABLE_BIT (1U << 10)

#define MMIO8(base,offset)  (*((volatile uint8_t*) ((uintptr_t)base+(uintptr_t)offset)))
#define MMIO16(base,offset) (*((volatile uint16_t*)((uintptr_t)base+(uintptr_t)offset)))
#define MMIO32(base,offset) (*((volatile uint32_t*)((uintptr_t)base+(uintptr_t)offset)))
#define MMIO64(base,offset) (*((volatile uint64_t*)((uintptr_t)base+(uintptr_t)offset)))

#define REG2MSR(reg) ((reg>>4)+0x800)//ex. 0x20 -> 0x802

using namespace ACPI;

extern MADT::Table * madt;

static bool x2apic;

static uint32_t lapic_base(){
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
        uint32_t base=lapic_base();
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
        uint32_t base=lapic_base();
        MMIO32(base,reg)=MMIO32(base,reg)|val;
    }
}

[[maybe_unused]]
static uint32_t ioapic_base(uint32_t n){
    for(uint32_t i=0;i<MADT::entry_count;i++){//NOTE maybe build an array of IOAPIC addresses instead of going through whole MADT table?
        if(MADT::entries[i]->type==MADT::Entry::IO_APIC){
            MADT::IOAPICEntry * entry=reinterpret_cast<MADT::IOAPICEntry*>(MADT::entries[i]);
            if(entry->id==n){
                return entry->address;
            }
        }
    }
    k_abort_fmt("can't find base address for inexistent IOAPIC #%d",n);
}

namespace APIC {
    
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
    
    static void lapic_init(){
        if(madt->flags&MADT::LEGACY_PIC){//mask out legacy PIC if necessary
            PIC::reset(0x20,0x28);
        }
        
        //enable LAPIC in case it isn't already enabled
        MSR::set(LAPIC_MSR,MSR::get(LAPIC_MSR)|LAPIC_MSR_ENABLE_BIT);
        
        //is x2apic enabled? shouldn't be, but check just in case
        x2apic=MSR::get(LAPIC_MSR)&LAPIC_MSR_X2APIC_ENABLE_BIT;
        
        if(!x2apic&&CPUID::has(CPUID::FEAT_ECX_1_x2APIC,0)){
            MSR::set(LAPIC_MSR,MSR::get(LAPIC_MSR)|LAPIC_MSR_X2APIC_ENABLE_BIT);
            x2apic=true;
        }
        
        //enable LAPIC's SIV (spurious interrupt vector) in case it isn't already enabled
        lapic_register_or(LAPIC_SIV,0x1FF);
        
    }
    
    void init(){
        Screen::write_s("\n -Enabling APIC...");
        
        lapic_init();//initialize LAPIC
        
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

