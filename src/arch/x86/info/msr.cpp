#include "arch/x86.h"
#include "screen.h"

void MSR::get(uint32_t msr, uint32_t *lo, uint32_t *hi){
    asm volatile("rdmsr" : "=a"(*lo), "=d"(*hi) : "c"(msr));
}

void MSR::set(uint32_t msr, uint32_t lo, uint32_t hi){
    asm volatile("wrmsr" : : "a"(lo), "d"(hi), "c"(msr));
}

void MSR::get(uint32_t msr, uint64_t *v){
    MSR::get(msr,reinterpret_cast<uint32_t*>(v),reinterpret_cast<uint32_t*>(v)+1);
}

uint64_t MSR::get(uint32_t msr){
    uint64_t v;
    MSR::get(msr,reinterpret_cast<uint32_t*>(&v),reinterpret_cast<uint32_t*>(&v)+1);
    return v;
}

void MSR::set(uint32_t msr, uint64_t v){
    MSR::set(msr,*reinterpret_cast<uint32_t*>(&v),*(reinterpret_cast<uint32_t*>(&v)+1));
}
