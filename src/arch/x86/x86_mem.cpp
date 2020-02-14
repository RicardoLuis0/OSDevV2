#include "arch/x86.h"
#include "mem.h"
#include "print.h"
#include "klib.h"

using namespace Memory;

extern uint8_t kernel_start;

extern uint8_t kernel_end;

namespace Memory::Internal{
    extern uint64_t total;
    extern uint64_t usable;
}

using namespace Memory::Internal;

constexpr uint64_t MM=(1024ULL*1024ULL);

constexpr uint32_t STACK_SIZE=32*(1024ULL);

void Memory::x86_init(struct multiboot_info * mbd){
    k_abort_s("Memory::x86_init unimplemented");
    print("\n -Parsing Memory Map...\n");
    /*
    memory_block * temp=nullptr;
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
                usable+=mmap->len;
                if(temp==nullptr){
                    build_first(mmap);
                    temp=block_root;
                }else{
                    temp->next=get_block((uint8_t*)mmap->addr,mmap->len,temp);
                    temp=temp->next;
                }
            }
        }
        if(!block_root){
            print("  .Memory ");
            Screen::setfgcolor(Screen::RED);
            print("FAIL");
            Screen::setfgcolor(Screen::WHITE);
            k_abort_s("Not Enough Memory");
            return;
        }
    }else{
        print("  .Memory ");
        Screen::setfgcolor(Screen::RED);
        print("FAIL");
        Screen::setfgcolor(Screen::WHITE);
        k_abort_s("Memory Map not available");
    }
    */
    print("  .Memory ");
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
}

