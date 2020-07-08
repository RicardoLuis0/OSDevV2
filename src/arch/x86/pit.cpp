#include "arch/x86.h"
#include "klib.h"
#include <stdatomic.h>

using namespace PIT;

constexpr uint64_t ms_to_hz(uint64_t ms){
    return 1000/ms;
}

constexpr uint16_t div_hz(uint64_t hz){
    return (1193181ull/hz)&0xffff;
}

atomic_int_least64_t PIT::timer;

void pit_interrupt(){
    PIT::timer++;
}

void PIT::init(){
    //TODO
}
