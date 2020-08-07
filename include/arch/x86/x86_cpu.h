#ifndef X86_CPU_H_INCLUDED
#define X86_CPU_H_INCLUDED

namespace CPU {
    
    struct CpuState{
        //TODO implement
    }__attribute__((packed));
    
    extern "C" [[noreturn]] void enter_state(CpuState*);
    extern "C" void switch_state(CpuState * from,CpuState * to);//switch to other state and save current state
    
    void generate_state(CpuState * data,void *(*start)(void*) ,void * stack, void * page_directory);
    
}

#endif // X86_CPU_H_INCLUDED
