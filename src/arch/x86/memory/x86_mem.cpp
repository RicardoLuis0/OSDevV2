#include "arch/x86.h"
#include "mem.h"
#include "print.h"
#include "klib.h"
#include "stdc/assert.h"
#include "util/spinlock.h"

using namespace Memory;

extern uint8_t kernel_start;

extern uint8_t kernel_end;

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
    if(reinterpret_cast<uint32_t>(ptr)%4096)k_abort_s("Can't get page from misaligned pointer");
    return reinterpret_cast<uint32_t>(ptr)/4096;
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

static inline void * map_virt_allow_fail(void * p,uint32_t n){
    uint32_t vp=next_free_virt_page_allow_fail(n);
    if(!vp)return nullptr;
    return to_ptr(map_virtual_page(to_page_id(p),vp,n));
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
    return nullptr;
}

static inline void * next_free_phys_pages_allow_fail(size_t n){
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
    return nullptr;
}

static inline void * next_free_phys_pages(size_t n){
    void * p=next_free_phys_pages_allow_fail(n);
    if(!p) k_abort_s("Out of Physical Memory");
    return p;
}

void * Memory::Internal::alloc_phys_page(uint32_t n){
    return register_phys_page(next_free_phys_pages(n),n);
}

void * Memory::Internal::alloc_phys_page_allow_fail(uint32_t n){
    void * p=next_free_phys_pages_allow_fail(n);
    if(!p)return nullptr;
    return register_phys_page(p,n);
}

void Memory::Internal::free_phys_page(void * p,uint32_t n){
    uint32_t id=to_page_id(p);
    while(n>0){
        if(is_free(id)){
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

void * Memory::alloc_virt_page_allow_fail(uint32_t n){
    void * p=Internal::alloc_phys_page_allow_fail(n);
    if(!p)return nullptr;
    return map_virt_allow_fail(p,n);
}

void Memory::free_virt_page(void * p,uint32_t n){
    Internal::free_phys_page(virt_to_phys(p),n);
    unmap_virt(p,n);
}

extern "C" uint64_t liballoc_heap_size();
extern "C" uint64_t liballoc_used();

constexpr uint32_t STACK_SIZE=32*(1024ULL);

constexpr uint64_t MM=(1024ULL*1024ULL);

void Memory::cmd_meminfo(){
    uint64_t mem_used=count_used_pages()*4096ULL;
    uint64_t mem_heap=liballoc_heap_size();
    uint64_t mem_heap_used=liballoc_used();
    uint64_t real_mem_used=(mem_used-mem_heap)+mem_heap_used;
    Screen::write_s("\nFree Physical Memory: ");
    Screen::setfgcolor(Screen::LIGHT_GREEN);
    Screen::write_mem(usable-mem_used);
    Screen::setfgcolor(Screen::WHITE);
    Screen::write_s("\nUsable Memory (free phys+free heap): ");
    Screen::setfgcolor(Screen::LIGHT_GREEN);
    Screen::write_mem(usable-real_mem_used);
    Screen::setfgcolor(Screen::WHITE);
    Screen::write_s("\nOccupied Physical Memory: ");
    Screen::setfgcolor(Screen::LIGHT_GREEN);
    Screen::write_mem(mem_used);
    Screen::setfgcolor(Screen::WHITE);
    Screen::write_s("\nKernel Binary Size: ");
    Screen::setfgcolor(Screen::LIGHT_GREEN);
    Screen::write_mem((reinterpret_cast<uint32_t>(&kernel_end)-reinterpret_cast<uint32_t>(&kernel_start))-STACK_SIZE);
    Screen::setfgcolor(Screen::WHITE);
    Screen::write_s("\nKernel Heap Size: ");
    Screen::setfgcolor(Screen::LIGHT_GREEN);
    Screen::write_mem(mem_heap);
    Screen::setfgcolor(Screen::WHITE);
    Screen::write_s("\nKernel Heap In Use: ");
    Screen::setfgcolor(Screen::LIGHT_GREEN);
    Screen::write_mem(mem_heap_used);
    Screen::setfgcolor(Screen::WHITE);
    Screen::write_s("\nNon-Heap In Use: ");
    Screen::setfgcolor(Screen::LIGHT_GREEN);
    Screen::write_mem(mem_used-liballoc_heap_size());
    Screen::setfgcolor(Screen::WHITE);
    //k_abort_s("meminfo unimplemented");
}

void Memory::x86_init(struct multiboot_info * mbd){
    Screen::write_s("\n -Parsing Memory Map...");
    struct blockdata{
        uint32_t start;
        uint32_t end;
    };
    constexpr uint8_t BLOCK_MAX = 16;
    blockdata blocks[BLOCK_MAX];
    uint8_t next_block=0;
    if(mbd&&mbd->flags&MULTIBOOT_INFO_MEM_MAP){
        void * mmap_max=reinterpret_cast<void*>(mbd->mmap_addr+mbd->mmap_length);
        for(multiboot_memory_map_t * mmap=reinterpret_cast<multiboot_memory_map_t *>(mbd->mmap_addr);mmap<mmap_max;mmap=reinterpret_cast<multiboot_memory_map_t *>(reinterpret_cast<uint32_t>(mmap)+mmap->size+sizeof(mmap->size))){
            total+=mmap->len;
            if(mmap->type!=MULTIBOOT_MEMORY_AVAILABLE){//ignore non-available memory
                continue;
            }else if(mmap->addr<MM){//don't bother with lower memory
                continue;
            }else if(mmap->addr>0xFFFFFFFFULL){//out of 32-bit addressing range
                continue;
            }else{
                if(next_block<BLOCK_MAX){
                    usable+=mmap->len;
                    blocks[next_block].start=static_cast<uint32_t>(mmap->addr);
                    blocks[next_block].end=static_cast<uint32_t>(mmap->addr)+mmap->len;
                    next_block++;
                }
            }
        }
        if(usable==0){
            //Screen::write_s("  .Memory Map ");
            Screen::setfgcolor(Screen::RED);
            Screen::write_s("FAIL");
            Screen::setfgcolor(Screen::WHITE);
            k_abort_s("No Usable Memory");
            return;
        }
    }else{
        //Screen::write_s("  .Memory Map ");
        Screen::setfgcolor(Screen::RED);
        Screen::write_s("FAIL");
        Screen::setfgcolor(Screen::WHITE);
        k_abort_s("Memory Map not available");
    }
    free_mem=usable;
    #ifdef DEBUG
    uint32_t page_count_expected=0;
    #endif // DEBUG
    for(uint32_t i=0;i<next_block;i++){
        uint32_t start=blocks[i].start/4096;
        uint32_t end=blocks[i].end/4096;
        if(end>pages.last_usable)pages.last_usable=end;
        #ifdef DEBUG
        page_count_expected+=(end-start)+1;
        #endif // DEBUG
        set_phys_free(start,end,true);
    }
    #ifdef DEBUG
    uint32_t page_count=0;
    for(uint32_t i=0;i<1048576;i++){
        if((pages.usage[i/32]>>(i%32))&0x1)page_count++;
    }
    if(page_count_expected!=page_count){
        print("Expected ",page_count_expected," pages, got ",page_count," pages\n");
    }
    assert(page_count_expected==page_count);
    #endif // DEBUG
    uint32_t k_start=reinterpret_cast<uint32_t>(&kernel_start);
    uint32_t k_end=reinterpret_cast<uint32_t>(&kernel_end);
    uint32_t k_start_page=k_start/4096;
    uint32_t k_end_page=(k_end/4096)+1;
    set_phys_free(k_start_page,k_end_page,false);
    Screen::setfgcolor(Screen::LIGHT_GREEN);
    Screen::write_s("OK\n");
    Screen::setfgcolor(Screen::WHITE);
    Screen::write_s("  .Usable: ");
    Screen::setfgcolor(Screen::LIGHT_GREEN);
    Screen::write_mem(usable);
    Screen::setfgcolor(Screen::WHITE);
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
        #ifdef LIBALLOC_ALLOW_NULL
            return alloc_virt_page_allow_fail(n);
        #else
            return alloc_virt_page(n);
        #endif // LIBALLOC_ALLOW_NULL
    }
    
    int liballoc_free(void * p,size_t n){
        free_virt_page(p,n);
        return 0;
    }
}
