#ifndef X86_IDT_H_INCLUDED
#define X86_IDT_H_INCLUDED

#include <stdint.h>
#include "util.h"

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
    
    
    struct regs {
        uint32_t eax;
        uint32_t ecx;
        uint32_t edx;
        uint32_t ebx;
        uint32_t esp;
        uint32_t ebp;
        uint32_t esi;
        uint32_t edi;
    };
    
    void setup();
    void init();
    void set_irq_handler(uint8_t irq,void(*)(void),gate_type,ring_type);//ignores data
    void set_irq_handler(uint8_t irq,void(*)(regs*),gate_type,ring_type);//receives register state
    void set_irq_handler(uint8_t irq,void(*)(uint32_t),gate_type,ring_type);//receives data
    void set_irq_handler(uint8_t irq,void(*)(uint32_t,regs*),gate_type,ring_type);//receives data and register state
    void set_irq_handler(uint8_t irq,void(*)(uint32_t,uint32_t),gate_type,ring_type);//receives data and irq number (for multi-irq handlers)
    void set_irq_handler(uint8_t irq,void(*)(uint32_t,uint32_t,regs*),gate_type,ring_type);//receives data and irq number (for multi-irq handlers)
    
    void set_exception_handler(uint8_t irq,void(*)(void),gate_type,ring_type);//ignores data
    void set_exception_handler(uint8_t irq,void(*)(regs*),gate_type,ring_type);//receives register state
    void set_exception_handler(uint8_t irq,void(*)(uint32_t),gate_type,ring_type);//receives data
    void set_exception_handler(uint8_t irq,void(*)(uint32_t,regs*),gate_type,ring_type);//receives data and register state
    void set_exception_handler(uint8_t irq,void(*)(uint32_t,uint32_t),gate_type,ring_type);//receives data and irq number (for multi-irq handlers)
    void set_exception_handler(uint8_t irq,void(*)(uint32_t,uint32_t,regs*),gate_type,ring_type);//receives data, irq number (for multi-irq handlers) and register state
    
    void set_raw_irq_handler(uint8_t irq,void * h,gate_type g,ring_type t);//h must be a pointer to a raw interrupt handler (must preserve state, send EOI and return with iret)
    void set_raw_exception_handler(uint8_t irq,void * h,gate_type g,ring_type t);//h must be a pointer to a raw interrupt handler (must preserve state, send EOI and return with iret)
    
    void disable_irq_handler(uint8_t irq);
    void disable_exception_handler(uint8_t irq);
    
    void irq_enable(uint8_t irq);
    void irq_disable(uint8_t irq);
    
    uint8_t irq_get_mapping(uint8_t irq);
    void irq_remap(uint8_t from,uint8_t to);
    
    bool irq_supports_remapping();
    bool irq_supports_remapping(uint8_t irq);
    
    template<uint32_t irq>
    void call_interrupt(uint32_t data){//call user-defined interrupt
        asm("int %1\n"
            :
            :"a"(data),
             "i"(irq));
    }
    
    void enable_interrupts();//safe to call if interrupts were already enabled/disabled
    void disable_interrupts();//safe to call if interrupts were already enabled/disabled
    
    void enable_nmi();
    void disable_nmi();
    
    //guards
    
    using interrupt_guard=Util::simple_generic_guard<disable_interrupts,enable_interrupts>;
    
    inline void __nmi_guard_acq(){
        disable_interrupts();
        disable_nmi();
    }
    
    inline void __nmi_guard_rel(){
        enable_nmi();
        enable_interrupts();
    }
    
    using nmi_guard=Util::simple_generic_guard<__nmi_guard_acq,__nmi_guard_rel>;
    
    void eoi();
    
    bool is_legacy_mode();
}

#endif // X86_IDT_H_INCLUDED
