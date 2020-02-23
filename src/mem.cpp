#include "klib.h"
#include "mem.h"
#include "screen.h"
#include "print.h"
#include "util/spinlock.h"
#include "util/vector.h"
#include "paging.h"

using namespace Memory;

namespace Memory::Internal {
    uint64_t total;
    uint64_t usable;
    page_t pages;
}

using namespace Memory::Internal;

static inline constexpr bool checkbit(uint32_t val,uint8_t offset){
    return (val>>offset)&0x1;
}

static inline bool has_free(uint16_t chunk){//check if chunk of 256 pages has any free pages
    return (pages.usage[chunk]!=0x0);
}

static inline void set_free(uint32_t page_id,bool new_free){
    if(new_free){
        pages.usage[page_id/256]|=(1<<page_id%256);
    }else{
        pages.usage[page_id/256]&=~(1<<page_id%256);
    }
}

static inline bool is_free(uint32_t page_id){
    return checkbit(pages.usage[page_id/256],page_id%256);
}

static inline bool is_free(uint16_t chunk,uint16_t offset){
    return checkbit(pages.usage[chunk],offset);
}

static inline void * to_ptr(uint16_t chunk,uint16_t offset){
    return reinterpret_cast<void*>(((chunk*256)+offset)*4096);
}

static inline void * to_ptr(uint32_t page){
    return reinterpret_cast<void*>(page*4096);
}

static inline uint32_t to_page_id(void * ptr){
    if(((uint32_t)ptr)%4096)k_abort_s("Can't get page from misaligned pointer");
    return ((uint32_t)ptr)/4096;
}

static inline void * virt_to_phys(void * v){
    return to_ptr(get_mapping_virt(to_page_id(v)));
}

static inline void * register_phys_page(void * p){
    uint32_t id=to_page_id(p);
    if(is_free(id)){
        set_free(id,false);
        return p;
    }else{
        k_abort_s("Trying to register non-free page");
        return nullptr;
    }
}

static inline void * map_virt(void * p,uint32_t n){
    return to_ptr(map_virtual_page(to_page_id(p),next_free_virt_page(),n));
}

static inline void unmap_virt(void * v,uint32_t n){
    unmap_virtual_page(to_page_id(v),n);
}

static inline void * next_free_phys_page(){
    for(uint16_t i=0;i<4096;i++){
        if(has_free(i)){
            for(uint16_t j=0;j<256;j++){
                if(is_free(i,j)){
                    return to_ptr(i,j);
                }
            }
        }
    }
    k_abort_s("OUT OF MEMORY");
    return nullptr;
}

void * Memory::alloc_phys_page(uint32_t n){
    if(n!=1)k_abort_s("multi-page allocation unimplemented");
    return register_phys_page(next_free_phys_page());
}

void Memory::free_phys_page(void * p,uint32_t n){
    if(n!=1)k_abort_s("multi-page de-allocation unimplemented");
    uint32_t id=to_page_id(p);
    if(is_free(id)){
        k_abort_s("Can't free an unused page");
    }else{
        set_free(id,true);
    }
}

void * Memory::alloc_virt_page(uint32_t n){
    if(n!=1)k_abort_s("multi-page allocation unimplemented");
    return map_virt(register_phys_page(next_free_phys_page()),1);
}

void Memory::free_virt_page(void * p,uint32_t n){
    free_phys_page(virt_to_phys(p),n);
    unmap_virt(p,n);
}

extern "C" void * k_malloc(size_t size){
    if(size<=4096)return alloc_virt_page(1);
    else k_abort_s("k_malloc unimplemented");
}

static inline void * zero_alloc(size_t size){
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
    if(size<=4096)return ptr;
    k_abort_s("k_realloc unimplemented");
}

extern "C" void k_free(void * ptr){
    free_virt_page(ptr,1);
}

void Memory::cmd_meminfo(){
    k_abort_s("meminfo unimplemented");
}

