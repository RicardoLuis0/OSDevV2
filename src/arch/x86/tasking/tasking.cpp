#include "arch/x86.h"
#include "arch/x86/tasking_internal.h"
#include "klib.h"

using namespace Tasking;

namespace Tasking {

    task_data * cpu_current_task;

    struct TSS {
        
    };

    void init(size_t cpu_count){
        cpu_current_task=new task_data[cpu_count];
    }

    void switch_task(task_data * other){
        const size_t cpu=get_cpu();
        cpu_current_task[cpu]=*other;
        other->cpu.restore();
    }

    size_t get_cpu(){
        k_abort_s("Tasking::get_cpu unimplemented");
    }

    void disable_scheduler(uint32_t core){
        k_abort_s("Tasking::disable_scheduler unimplemented");
    }

    void enable_scheduler(uint32_t core){
        k_abort_s("Tasking::enable_scheduler unimplemented");
    }

}
