#include "klib.h"
#include "mem.h"
#include "screen.h"
#include "print.h"
#include "util/spinlock.h"
#include "util/vector.h"

using namespace Memory;

const uint32_t Memory::page_size=4096;//4K pages

namespace Memory::Internal {
    uint64_t total;
    uint64_t usable;
    struct {
        uint32_t usage[4096];//4GB of page usage, each page is 1 bit
    } pages;
}

using namespace Memory::Internal;

inline constexpr bool checkbit(uint32_t val,uint8_t offset){
    return (val>>offset)&0x1;
}

static inline bool has_free(uint16_t chunk){//check if chunk of 256 pages has any free pages
    return (pages.usage[chunk]!=0x0);
}

static void set_free(uint32_t page_id,bool new_free){
    if(new_free){
        pages.usage[page_id/256]|=(1<<page_id%256);
    }else{
        pages.usage[page_id/256]&=~(1<<page_id%256);
    }
}

static inline bool is_used(uint32_t page_id){
    return !checkbit(pages.usage[page_id/256],page_id%256);
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

static uint32_t to_page_id(void * ptr){
    if(((uint32_t)ptr)%4096)k_abort_s("Can't get page from misaligned pointer");
    return ((uint32_t)ptr)/4096;
}

static void * virt_to_phys(void * p){
    return p;//TODO
}

static void * map_virt_page(void * p,void * a,uint32_t range){
    return p;//TODO
}

static void unmap_virt_page(void * p){
    //TODO
}

static void * next_free_phys_page(){
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

static void * register_phys_page(void * p){
    uint32_t id=to_page_id(p);
    if(is_free(id)){
        set_free(id,false);
        return p;
    }else{
        k_abort_s("Trying to register non-free page");
        return nullptr;
    }
}

void * Memory::alloc_phys_page(){
    return register_phys_page(next_free_phys_page());
}

void Memory::free_phys_page(void * p){
    uint32_t id=to_page_id(p);
    if(is_used(id)){
        set_free(id,true);
    }else{
        k_abort_s("Can't free an unused page");
    }
}

void * Memory::alloc_virt_page(){
    return map_virt_page(register_phys_page(next_free_phys_page()),nullptr,1);
}

void Memory::free_virt_page(void * p){
    free_phys_page(virt_to_phys(p));
    unmap_virt_page(p);
}

extern "C" void * k_malloc(size_t size){
    if(size<=page_size)return alloc_virt_page();
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
    if(size<=page_size)return ptr;
    k_abort_s("k_realloc unimplemented");
}

extern "C" void k_free(void * ptr){
    free_virt_page(ptr);
}

void Memory::cmd_meminfo(){
    k_abort_s("meminfo unimplemented");
}

