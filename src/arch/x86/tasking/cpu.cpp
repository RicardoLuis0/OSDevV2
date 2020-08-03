#include "arch/x86.h"
#include "klib.h"

namespace CPU {
    
    void generate_state(CpuState * data,void *(*start)(void*) ,void * stack, void * page_directory);
    
}
