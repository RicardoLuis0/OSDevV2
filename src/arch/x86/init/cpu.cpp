#include "arch/x86.h"
#include "klib.h"

void CPU::save(cpu_state*){
    k_abort_s("CPU::save_state unimplemented");
}

void CPU::restore(cpu_state*){
    k_abort_s("CPU::restore_state unimplemented");
}
