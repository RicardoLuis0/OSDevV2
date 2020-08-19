#ifndef X86_IDT_H_INCLUDED
#define X86_IDT_H_INCLUDED

#include <stdint.h>

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
    
    void irq_enable(uint8_t irq);
    void irq_disable(uint8_t irq);
    
    template<uint32_t irq>
    void call_interrupt(uint32_t data){//call user-defined interrupt
        asm("int %1\n"
            :
            :"a"(data),
             "i"(irq));
    }
    
    void enable_interrupts();//safe to call if interrupts were already enabled/disabled
    void disable_interrupts();//safe to call if interrupts were already enabled/disabled
    
    class interrupt_guard {
        interrupt_guard(const interrupt_guard& other)=delete;
        interrupt_guard(interrupt_guard&& other)=delete;
        interrupt_guard& operator=(const interrupt_guard& other)=delete;
        interrupt_guard& operator=(interrupt_guard&& other)=delete;
    public:
        inline interrupt_guard(){
            disable_interrupts();
        }
        inline ~interrupt_guard(){
            enable_interrupts();
        }
    };
    
}

#endif // X86_IDT_H_INCLUDED
