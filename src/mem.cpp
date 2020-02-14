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

extern "C" void * k_malloc(size_t size){
    k_abort_s("k_mealloc unimplemented");
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
    k_abort_s("k_free unimplemented");
}

void Memory::cmd_meminfo(){
    k_abort_s("meminfo unimplemented");
}

