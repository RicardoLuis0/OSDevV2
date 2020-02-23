#include "arch/x86.h"
#include "mem.h"
#include "print.h"
#include "klib.h"
#include "paging.h"

using namespace Memory;

extern uint8_t kernel_start;

extern uint8_t kernel_end;

namespace Memory::Internal{
    extern uint64_t total;
    extern uint64_t usable;
    extern page_t pages;
}

using namespace Memory::Internal;

static inline void mark_used(uint32_t page_id){//mark as used
    pages.usage[page_id/256]&=~(1<<page_id%256);
}

static inline void mark_free(uint32_t page_id){//mark as free
    pages.usage[page_id/256]|=(1<<page_id%256);
}

constexpr uint64_t MM=(1024ULL*1024ULL);

constexpr uint32_t STACK_SIZE=32*(1024ULL);

constexpr uint32_t large_align=4096*256;

static inline constexpr bool is_large_aligned(uint32_t n){
    return !(n%large_align);
}

static inline constexpr uint32_t prev_align(uint32_t n){
    return n-(n%large_align);
}

static inline constexpr uint32_t next_align(uint32_t n){
    return n+(large_align-(n%large_align));
}

void Memory::x86_init(struct multiboot_info * mbd){
    print("\n -Parsing Memory Map...\n");
    struct blockdata{
        uint32_t start;
        uint32_t end;
    };
    constexpr uint8_t BLOCK_MAX = 16;
    blockdata blocks[BLOCK_MAX];
    uint8_t next_block=0;
    if(mbd&&mbd->flags&MULTIBOOT_INFO_MEM_MAP){
        void * mmap_max=(void*)(mbd->mmap_addr+mbd->mmap_length);
        for(multiboot_memory_map_t * mmap=(multiboot_memory_map_t *)mbd->mmap_addr;mmap<mmap_max;mmap=(multiboot_memory_map_t *)(((uint32_t)mmap)+mmap->size+sizeof(mmap->size))){
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
                    blocks[next_block].start=(uint32_t)mmap->addr;
                    blocks[next_block].end=(uint32_t)mmap->addr+mmap->len;
                    next_block++;
                }
            }
        }
        if(usable==0){
            print("  .Memory Map ");
            Screen::setfgcolor(Screen::RED);
            print("FAIL");
            Screen::setfgcolor(Screen::WHITE);
            k_abort_s("No Usable Memory");
            return;
        }
    }else{
        print("  .Memory Map ");
        Screen::setfgcolor(Screen::RED);
        print("FAIL");
        Screen::setfgcolor(Screen::WHITE);
        k_abort_s("Memory Map not available");
    }
    /*
    for(uint32_t i=0;i<4096;i++){
        pages.usage[i]=0x0U;
    }
    */
    for(uint32_t i=0;i<next_block;i++){
        if(!is_large_aligned(blocks[i].start)){
            uint32_t start=blocks[i].start;
            uint32_t end=next_align(start);
            blocks[i].start=end;
            start/=4096;
            end/=4096;
            for(;start<end;start++){
                mark_free(start);
            }
        }
        if(blocks[i].start<blocks[i].end){
            uint32_t start=blocks[i].start;
            uint32_t end=prev_align(blocks[i].end);
            blocks[i].start=end;
            start/=large_align;
            end/=large_align;
            for(;start<end;start++){
                pages.usage[start]=0xFFFFFFFFU;
            }
            if(blocks[i].start<blocks[i].end){
                uint32_t start=blocks[i].start/4096;
                uint32_t end=blocks[i].end/4096;
                for(;start<end;start++){
                    mark_free(start);
                }
            }
        }
    }
    uint32_t k_start=((uint32_t)&kernel_start);
    uint32_t k_end=((uint32_t)&kernel_end);
    uint32_t k_start_page=k_start/4096;
    uint32_t k_end_page=(k_end/4096)+1;
    for(uint32_t i=k_start_page;i<=k_end_page;i++){
        mark_used(i);
    }
    print("  .Memory Map ");
    Screen::setfgcolor(Screen::LIGHT_GREEN);
    print("OK");
    Screen::setfgcolor(Screen::WHITE);
    print(", Usable: ");
    Screen::setfgcolor(Screen::LIGHT_GREEN);
    Screen::write_mem(usable);
    Screen::setfgcolor(Screen::WHITE);
    Screen::write_s("\n  .Kernel: ");
    Screen::setfgcolor(Screen::LIGHT_GREEN);
    Screen::write_mem((((uint32_t)&kernel_end)-((uint32_t)&kernel_start))-STACK_SIZE);//size of kernel in memory minus stack size
    Screen::setfgcolor(Screen::WHITE);
    Screen::write_s(" / Stack:  ");
    Screen::setfgcolor(Screen::LIGHT_GREEN);
    Screen::write_mem(STACK_SIZE);//32K
    Screen::setfgcolor(Screen::WHITE);
    Screen::write_s(" / Total:  ");
    Screen::setfgcolor(Screen::LIGHT_GREEN);
    Screen::write_mem((((uint32_t)&kernel_end)-((uint32_t)&kernel_start)));//??
    Screen::setfgcolor(Screen::WHITE);
    paging_init();
}

