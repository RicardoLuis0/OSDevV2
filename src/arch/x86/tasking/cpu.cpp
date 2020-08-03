#include "arch/x86.h"
#include "klib.h"

namespace CPU {
    
    void CpuState::save(){
        k_abort_s("CPU::CpuState::save unimplemented");
    }
    
    void CpuState::new_at(void * code,void * stack, void * page_directory){
        k_abort_s("CPU::CpuState::new_at unimplemented");
    }
    
    [[noreturn]] void CpuState::restore(){
        k_abort_s("CPU::CpuState::restore unimplemented");
    }
    
}
