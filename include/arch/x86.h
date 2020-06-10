#ifndef X86_H_INCLUDED
#define X86_H_INCLUDED

#include "stdc/stdint.h"
#include "extern/multiboot.h"


extern "C" uint32_t has_cpuid();

namespace CPUID {
    void check();
}

namespace Memory{
    void x86_init(multiboot_info * mbd);
}

namespace Screen{
    void x86_init();
}

namespace GDT {
    void init();
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
        RING_0,
        RING_1,
        RING_2,
        RING_3,
    };
    void init();
    void set_irq_handler(uint8_t num,void(*)(void),gate_type,ring_type);//ignores data
    void set_irq_handler(uint8_t num,void(*)(uint32_t),gate_type,ring_type);//receives data
    void set_irq_handler(uint8_t num,void(*)(uint32_t,uint32_t),gate_type,ring_type);//receives data and irq number (for multi-irq handlers)
    void set_exception_handler(uint8_t num,void(*)(void),gate_type,ring_type);//ignores data
    void set_exception_handler(uint8_t num,void(*)(uint32_t),gate_type,ring_type);//receives data
    void set_exception_handler(uint8_t num,void(*)(uint32_t,uint32_t),gate_type,ring_type);//receives data and irq number (for multi-irq handlers)
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

#endif // X86_H_INCLUDED
