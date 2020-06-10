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
    uint64_t free_mem;
    uint64_t phys_mem_in_use;
    physical_pages_t pages;
}

using namespace Memory::Internal;

static inline constexpr bool checkbit(uint32_t val,uint8_t offset){
    return (val>>offset)&0x1;
}

static inline bool has_free(uint32_t chunk){//check if chunk of 64 pages has any free pages
    return (pages.usage[chunk]!=0);
}

static inline void set_free(uint32_t page_id,bool new_free){
    if(new_free){
        pages.usage[page_id/32]|=(1<<(page_id%32));
    }else{
        pages.usage[page_id/32]&=~(1<<(page_id%32));
    }
}

void Memory::Internal::set_phys_free(uint32_t start,uint32_t end,bool new_free){
    uint32_t start2=(start%32)?start:(start+(32-start%32));
    uint32_t end2=end-end%32;
    if(start2>end2){
        if(start!=start2){
            for(uint32_t i=start;i<start2;i++){
                set_free(start+i,new_free);
            }
            start=start2;
        }
        if(end!=end2){
            for(uint32_t i=end2;i<=end;i++){
                set_free(i,new_free);
            }
            end=end2;
        }
        if(start>end){
            for(;start<end;start+=32){
                if(new_free){
                    pages.usage[start/32]=0xFFFFFFFFU;
                }else{
                    pages.usage[start/32]=0;
                }
            }
        }
    }else{
        for(;start<=end;start++){
            set_free(start,new_free);
        }
    }
}

static inline bool is_free(uint32_t page_id){
    return checkbit(pages.usage[page_id/32],page_id%32);
}

bool Memory::Internal::is_phys_page_free(uint32_t page_id){
    return is_free(page_id);
}

static inline bool is_free(uint16_t chunk,uint8_t offset){
    return checkbit(pages.usage[chunk],offset);
}

static inline bool is_free(uint16_t chunk,uint8_t offset,size_t length,uint16_t &chunk_out,uint8_t &offset_out){
    uint32_t free_pgs=0;
    for(;chunk<phys_page_segment;chunk++){
        for(;offset<32;offset++){
            if(is_free(chunk,offset)){
                free_pgs++;
            }else{
                chunk_out=chunk;
                offset_out=offset;
                return false;
            }
            if(free_pgs==length)return true;
        }
        offset=0;
    }
    chunk_out=phys_page_segment;
    offset_out=32;
    return false;
}

static inline void * to_ptr(uint16_t chunk,uint8_t offset){
    return reinterpret_cast<void*>(((chunk*32)+offset)*4096);
}

static inline void * to_ptr(uint32_t page){
    return reinterpret_cast<void*>(page*4096);
}

void * Memory::Internal::phys_id_to_ptr(uint32_t page_id){
    return to_ptr(page_id);
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

static inline void * register_phys_page(void * p,size_t n){
    if(n==1)return register_phys_page(p);
    uint32_t id=to_page_id(p);
    while(n>0){
        if(is_free(id)){
            set_free(id,false);
        }else{
            k_abort_s("Trying to register non-free page");
            return nullptr;
        }
        n--;
        id++;
    }
    return p;
}

static inline void * map_virt(void * p,uint32_t n){
    return to_ptr(map_virtual_page(to_page_id(p),next_free_virt_page(n),n));
}

static inline void unmap_virt(void * v,uint32_t n){
    unmap_virtual_page(to_page_id(v),n);
}

static inline void * next_free_phys_page(){
    for(uint16_t i=0;i<phys_page_segment;i++){
        if(has_free(i)){
            for(uint8_t j=0;j<32;j++){
                if(is_free(i,j)){
                    return to_ptr(i,j);
                }
            }
        }
    }
    k_abort_s("OUT OF MEMORY");
    return nullptr;
}

static inline void * next_free_phys_pages(size_t n){
    if(n==1)return next_free_phys_page();
    for(uint16_t i=0;i<phys_page_segment;i++){
        if(has_free(i)){
            for(uint8_t j=0;j<32;j++){
                uint16_t i2;
                uint8_t j2;
                if(is_free(i,j,n,i2,j2)){
                    return to_ptr(i,j);
                }else{
                    i=i2;
                    j=j2;
                    if(!has_free(i)){
                        break;
                    }
                }
            }
        }
    }
    k_abort_s("OUT OF MEMORY");
    return nullptr;
}

void * Memory::Internal::alloc_phys_page(uint32_t n){
    return register_phys_page(next_free_phys_pages(n),n);
}

void Memory::Internal::free_phys_page(void * p,uint32_t n){
    uint32_t id=to_page_id(p);
    while(n>0){
        if(!is_phys_page_free(id)){
            k_abort_s("Can't free an unused page");
        }else{
            set_free(id,true);
        }
        n--;
        id++;
    }
}

uint32_t Memory::Internal::count_used_pages(){//highest index of used page
    uint32_t count=0;
    for(int i=0;i<pages.last_usable;i++){//TODO optimize
        if(!is_free(i))count++;
    }
    return count;
}

void * Memory::alloc_virt_page(uint32_t n){
    return map_virt(Internal::alloc_phys_page(n),n);
}

void Memory::free_virt_page(void * p,uint32_t n){
    Internal::free_phys_page(virt_to_phys(p),n);
    unmap_virt(p,n);
}

extern "C" uint64_t liballoc_heap_size();
extern "C" uint64_t liballoc_used();

void Memory::cmd_meminfo(){
    uint64_t mem_used=count_used_pages()*4096ULL;
    uint64_t mem_heap=liballoc_heap_size();
    uint64_t mem_heap_used=liballoc_used();
    uint64_t real_mem_used=(mem_used-mem_heap)+mem_heap_used;
    k_puts("\nFree Physical Memory: ");
    Screen::setfgcolor(Screen::LIGHT_GREEN);
    k_putmem(usable-mem_used);
    Screen::setfgcolor(Screen::WHITE);
    k_puts("\nUsable Memory (+free heap space): ");
    Screen::setfgcolor(Screen::LIGHT_GREEN);
    k_putmem(usable-real_mem_used);
    Screen::setfgcolor(Screen::WHITE);
    k_puts("\nOccupied Physical Memory: ");
    Screen::setfgcolor(Screen::LIGHT_GREEN);
    k_putmem(mem_used);
    Screen::setfgcolor(Screen::WHITE);
    k_puts("\nKernel Heap Size: ");
    Screen::setfgcolor(Screen::LIGHT_GREEN);
    k_putmem(mem_heap);
    Screen::setfgcolor(Screen::WHITE);
    k_puts("\nKernel Heap In Use: ");
    Screen::setfgcolor(Screen::LIGHT_GREEN);
    k_putmem(mem_heap_used);
    Screen::setfgcolor(Screen::WHITE);
    k_puts("\nNon-Heap In Use: ");
    Screen::setfgcolor(Screen::LIGHT_GREEN);
    k_putmem(mem_used-liballoc_heap_size());
    Screen::setfgcolor(Screen::WHITE);
    //k_abort_s("meminfo unimplemented");
}


Util::Spinlock _liballoc_spinlock;

extern "C" {
    
    //liballoc support functions
    
    int liballoc_lock(){
        _liballoc_spinlock.lock();
        return 0;
    }
    
    int liballoc_unlock(){
        _liballoc_spinlock.release();
        return 0;
    }
    
    void * liballoc_alloc(size_t n){
        return alloc_virt_page(n);
    }
    
    int liballoc_free(void * p,size_t n){
        free_virt_page(p,n);
        return 0;
    }
}
