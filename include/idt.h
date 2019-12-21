#ifndef IDT_H_INCLUDED
#define IDT_H_INCLUDED

#include "default/stdint.h"

namespace IDT{
    void init();
    void set_handler(uint8_t num,void(*)(void));
}

#endif // IDT_H_INCLUDED
