#include "arch/x86.h"

void MSR::get(uint32_t msr, uint32_t *lo, uint32_t *hi){
    asm volatile("rdmsr" : "=a"(*lo), "=d"(*hi) : "c"(msr));
}

void MSR::set(uint32_t msr, uint32_t lo, uint32_t hi){
    asm volatile("wrmsr" : : "a"(lo), "d"(hi), "c"(msr));
}

void MSR::get(uint32_t msr, uint64_t *v){
    MSR::get(msr,(uint32_t*)v,((uint32_t*)v)+1);
}

void MSR::set(uint32_t msr, uint64_t v){
    MSR::set(msr,*((uint32_t*)v),*(((uint32_t*)v)+1));
}
