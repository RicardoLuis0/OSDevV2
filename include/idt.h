#ifndef IDT_H_INCLUDED
#define IDT_H_INCLUDED

#include "stdc/stdint.h"

namespace IDT{
    enum gate_type {
        G_32_INT=0b1110,
        G_32_TASK=0b0101,
        G_32_TRAP=0b1111,
        //G_16_INT=0b0110,
        //G_16_TRAP=0b0111,
    };
    enum ring_type {
        RING_0,
        RING_1,
        RING_2,
        RING_3,
    };
    void init();
    void set_irq_handler(uint8_t num,void(*)(void),gate_type,ring_type);
    void set_irq_handler(uint8_t num,void(*)(int),gate_type,ring_type);
    void set_irq_handler(uint8_t num,void(*)(int,int),gate_type,ring_type);
    void set_exception_handler(uint8_t num,void(*)(void),gate_type,ring_type);
    void set_exception_handler(uint8_t num,void(*)(int),gate_type,ring_type);
    void set_exception_handler(uint8_t num,void(*)(int,int),gate_type,ring_type);
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

#endif // IDT_H_INCLUDED
