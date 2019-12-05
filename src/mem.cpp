#include "klib.h"
#include "mem.h"

extern "C" void * k_malloc(uint32_t size){
    k_abort_s("Memory not implemented yet");
    return NULL;
}

void Memory::init(){
}
