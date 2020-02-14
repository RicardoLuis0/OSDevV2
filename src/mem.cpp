#include "klib.h"
#include "mem.h"
#include "screen.h"
#include "print.h"
#include "util/spinlock.h"
#include "util/vector.h"

using namespace Memory;

namespace Memory::Internal {
    uint64_t total;
    uint64_t usable;
}
using namespace Memory::Internal;


void * Memory::alloc_page(){
    k_abort_s("Memory::alloc_page unimplemented");
}

void Memory::free_page(void*){
    k_abort_s("Memory::free_page unimplemented");
}

extern "C" void * k_malloc(size_t size){
    if(size<4096)return alloc_page();
    else k_abort_s("k_malloc unimplemented");
}

static void * zero_alloc(size_t size){
    void * p=k_malloc(size);
    if(p){
        k_memset(p,0,size);
    }
    return p;
}

extern "C" void * k_calloc(size_t num,size_t size){
    return zero_alloc(size*num);
}

extern "C" void * k_realloc(void * ptr,size_t size){
    k_abort_s("k_realloc unimplemented");
}

extern "C" void k_free(void * ptr){
    free_page(ptr);
}

void Memory::cmd_meminfo(){
    k_abort_s("meminfo unimplemented");
}

