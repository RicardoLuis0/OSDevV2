#ifndef X86_TASKING_H_INCLUDED
#define X86_TASKING_H_INCLUDED

#include <stdint.h>
#include <stddef.h>

namespace Tasking{
    
    struct task_data;
    
    void init(size_t cpu_count);
    
    [[noreturn]] void switch_task(task_data * other);
    
    size_t get_cpu();
    
    void disable_scheduler(uint32_t cpu);
    void enable_scheduler(uint32_t cpu);
    
    //task_data * schedule(/* ??? */);
    
}


#endif // X86_TASKING_H_INCLUDED
