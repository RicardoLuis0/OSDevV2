#ifndef X86_CPU_H_INCLUDED
#define X86_CPU_H_INCLUDED

namespace CPU {
    struct CpuState{
        //TODO implement
    }__attribute__((packed));
    extern "C" [[noreturn]] void enter_state(CpuState*);
    void generate_state(CpuState * data,void *(*start)(void*) ,void * stack, void * page_directory);
}

#endif // X86_CPU_H_INCLUDED
