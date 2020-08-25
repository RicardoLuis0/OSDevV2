#include "arch/x86.h"
#include "klib.h"
#include "screen.h"

namespace APIC {
    void init(){
        Screen::write_s("\n -Enabling APIC...");
        k_abort_s("APIC::init unimplemented");
    }
    
    bool supported(){
        return false;//CPUID::has(0,CPUID::FEAT_EDX_APIC);
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

