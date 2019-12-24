#ifndef IDT_H_INCLUDED
#define IDT_H_INCLUDED

#include "default/stdint.h"

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
    void set_handler(uint8_t num,void(*)(int),gate_type,ring_type);
}

#endif // IDT_H_INCLUDED
