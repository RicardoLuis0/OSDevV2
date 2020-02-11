#include "klib.h"
#include "mem.h"
#include "screen.h"
#include "print.h"
#include "util/spinlock.h"

using namespace Memory;

namespace Memory::Internal{

    Util::Spinlock memory_lock;

    memory_block * block_root=nullptr;

    memory_block * block_unused=nullptr;//unused memory blocks, start, end and type are garbage, but next and prev are guaranteed to be either a valid unused block or nullptr

    uint64_t total=0;

    uint64_t usable=0;

    void build_unused(uint8_t * data,uint32_t count){//count>0
        memory_block * temp=(memory_block *)data;
        temp->next=block_unused;
        temp->prev=nullptr;
        data+=sizeof(memory_block);
        for(uint32_t i=1;i<count;i++,data+=sizeof(memory_block)){
            temp->prev=(memory_block*)data;
            temp->prev->next=temp;
            temp->prev->prev=nullptr;
            temp=temp->prev;
        }
        block_unused=temp;
    }

    memory_block * get_unused_block(){
        if(block_unused==nullptr){
            uint8_t * tmp=(uint8_t*)k_malloc(sizeof(memory_block)*100);
            if(!tmp){
                k_abort_s("Kernel Out of Memory!");
            }else{
                build_unused(tmp,100);
            }
        }
        memory_block * temp=block_unused;
        block_unused=block_unused->next;
        if(block_unused){
            block_unused->prev=nullptr;
        }
        return temp;
    }

    memory_block * get_block(uint8_t * start,uint32_t size,memory_block * prev=nullptr,memory_block * next=nullptr){
        if(((uint32_t)start)%16!=0){
            uint8_t* start_bak=start;
            start=(uint8_t *)((((uint32_t)start) + 15) & ~15);
            size-=start-start_bak;
        }
        memory_block * temp=get_unused_block();
        temp->start=start;
        temp->size=size;
        temp->prev=prev;
        temp->next=next;
        temp->type=memory_block::MEMORY_BLOCK_FREE;
        return temp;
    }


    memory_block * split(memory_block * block,uint32_t size){
        if(size==block->size){
            block->type=memory_block::MEMORY_BLOCK_USED;
            return block;
        }else if(size>block->size){
            k_abort_s("Memory::split size too large");
        }
        memory_block * temp=get_block(block->start+size,block->size-size,block,block->next);
        block->next=temp;
        block->size=size;
        block->type=memory_block::MEMORY_BLOCK_USED;
        return block;
    }

    memory_block * merge(memory_block * first,memory_block * second){
        first->size+=second->size;
        first->next=second->next;
        if(first->next){
            first->next->prev=first;
        }
        second->prev=nullptr;
        second->next=block_unused;
        block_unused=second;
        return first;
    }

}

using namespace Memory::Internal;

memory_block * Memory::alloc_block(uint32_t size){//first fit
    if(size%16!=0){
        size=(size + 15) & ~15;
    }
    Util::SpinlockGuard guard(memory_lock);
    memory_block * block;
    for(block=block_root;block!=nullptr&&(block->size<size||block->type!=memory_block::MEMORY_BLOCK_FREE);block=block->next);
    if(block==nullptr){
        return nullptr;//out of memory
    }else{
        return split(block,size);
    }
}

void Memory::free_block(memory_block * ptr){
    Util::SpinlockGuard guard(memory_lock);
    if(ptr->type==memory_block::MEMORY_BLOCK_USED){
        ptr->type=memory_block::MEMORY_BLOCK_FREE;
        if(ptr->prev){
            if(((ptr->prev->start+ptr->prev->size)==ptr->start)&&ptr->next->type==memory_block::MEMORY_BLOCK_FREE){
                ptr=merge(ptr->prev,ptr);
            }
            if(((ptr->start+ptr->size)==ptr->next->start)&&ptr->next->type==memory_block::MEMORY_BLOCK_FREE){
                ptr=merge(ptr,ptr->next);
            }
        }
    }else{
        //do nothing
    }
}

Memory::memory_block * find_block(void * ptr){
    Memory::memory_block * block=block_root;
    while(block!=nullptr){
        if(block->start==ptr){//ptr found
            return block->type==Memory::memory_block::MEMORY_BLOCK_USED?block:nullptr;
        }else if(ptr<block->start){
            return nullptr;//ptr not valid
        }
        block=block->next;
    }
    return nullptr;
}

extern "C" void * k_malloc(size_t size){
    if(Memory::memory_block * b=Memory::alloc_block(size)){
        return b->start;
    }else{
        return nullptr;
    }
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
    void * p2=nullptr;
    Memory::memory_block * block;
    if((block=find_block(ptr))&&block->type==Memory::memory_block::MEMORY_BLOCK_USED){
        p2=zero_alloc(size);
        if(p2){
            k_memcpy(p2,ptr,min(size,block->size));
            free_block(block);
        }
    }else{
        p2=zero_alloc(size);
    }
    return p2;
}

extern "C" void k_free(void * ptr){
    Memory::memory_block * block=find_block(ptr);
    if(block!=nullptr){
        Memory::free_block(block);
    }
}

void Memory::cmd_meminfo(){
    uint64_t free=0;
    for(memory_block * b=block_root;b!=nullptr;b=b->next){
        if(b->type==Memory::memory_block::MEMORY_BLOCK_FREE){
            free+=b->size;
        }
    }
    Screen::setfgcolor(Screen::WHITE);
    print("\nFree: ");
    Screen::setfgcolor(Screen::LIGHT_GREEN);
    Screen::write_mem(free);
    Screen::setfgcolor(Screen::WHITE);
    print("\nUsed: ");
    Screen::setfgcolor(Screen::LIGHT_GREEN);
    Screen::write_mem(usable-free);
    Screen::setfgcolor(Screen::WHITE);
    print("\nAvailable: ");
    Screen::setfgcolor(Screen::LIGHT_GREEN);
    Screen::write_mem(usable);
    Screen::setfgcolor(Screen::WHITE);
    print("\nTotal: ");
    Screen::setfgcolor(Screen::LIGHT_GREEN);
    Screen::write_mem(total);
    Screen::setfgcolor(Screen::WHITE);
}
