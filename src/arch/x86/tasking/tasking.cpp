#include "arch/x86.h"
#include "arch/x86/tasking_internal.h"
#include "klib.h"

using namespace Tasking;


struct TSS {
    
};

void Tasking::init(){
    
}

void Tasking::switch_task(task_data * other){
    k_abort_s("Tasking::switch_task unimplemented");
}

uint32_t Tasking::get_core(){
    k_abort_s("Tasking::get_core unimplemented");
}

void Tasking::disable_scheduler(uint32_t core){
    k_abort_s("Tasking::disable_scheduler unimplemented");
}

void Tasking::enable_scheduler(uint32_t core){
    k_abort_s("Tasking::enable_scheduler unimplemented");
}
