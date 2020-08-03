#ifndef X86_PIT_H_INCLUDED
#define X86_PIT_H_INCLUDED

#include <stdint.h>
#include <stdatomic.h>

namespace PIT {
    void init();
    extern volatile atomic_int_least64_t timer;
    extern const uint32_t timer_resolution;//timer resolution in ms
}

#endif // X86_PIT_H_INCLUDED
