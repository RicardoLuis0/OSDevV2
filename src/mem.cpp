#include "extern/multiboot.h"
#include "klib.h"
#include "mem.h"
#include "screen.h"
#include "print.h"

using namespace Memory;

struct gdt_entry{
    uint8_t arr[8];
};

static inline gdt_entry encodeGdtEntry(uint32_t base, uint32_t limit, uint32_t type) {
    //adapted from https://wiki.osdev.org/GDT_Tutorial, accessed on decembeer 5 2019
    // Check the limit to make sure that it can be encoded
    gdt_entry target;
    if ((limit > 65536) && ((limit & 0xFFF) != 0xFFF)) {
        k_abort_s("Invalid limit for GDT entry!");
    }
    if (limit > 65536) {
        // Adjust granularity if required
        limit = limit >> 12;
        target.arr[6] = 0xC0;
    } else {
        target.arr[6] = 0x40;
    }
    // Encode the limit
    target.arr[0] = limit & 0xFF;
    target.arr[1] = (limit >> 8) & 0xFF;
    target.arr[6] |= (limit >> 16) & 0xF;
    // Encode the base 
    target.arr[2] = base & 0xFF;
    target.arr[3] = (base >> 8) & 0xFF;
    target.arr[4] = (base >> 16) & 0xFF;
    target.arr[7] = (base >> 24) & 0xFF;
    // And... Type
    target.arr[5] = type;
    return target;
}

gdt_entry GDT[3];

extern "C" void loadgdt(uint32_t base,uint16_t limit);

extern "C" void * k_malloc(uint32_t size){
    return NULL;
}

extern uint8_t kernel_start;

extern uint8_t kernel_end;

constexpr uint64_t MM=(1024ULL*1024ULL);

memory_block * block_root=nullptr;

memory_block * block_unused=nullptr;//unused memory blocks, start, end and type are garbage, but next and prev are guaranteed to be either a valid unused block or nullptr

memory_block * get_unused_block(){
    if(block_unused!=nullptr){
        memory_block * temp=block_unused;
        block_unused=block_unused->next;
        if(block_unused){
            block_unused->prev=nullptr;
        }
        return temp;
    }else{
        k_abort_s("Out of memory blocks");
        return nullptr;
    }
}

memory_block * get_block(uint8_t * start,uint32_t size,memory_block * prev=nullptr,memory_block * next=nullptr){
    memory_block * temp=get_unused_block();
    temp->start=start;
    temp->size=size;
    temp->prev=prev;
    temp->next=next;
    return temp;
}

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

void build_first(multiboot_memory_map_t * mmap){
    uint8_t * start=(uint8_t*)mmap->addr;
    uint8_t * end=(uint8_t*)(mmap->addr+mmap->len);
    if(start>=&kernel_start&&end<=&kernel_end){
        return;//invalid, fully inside kernel
    }else if(start>=&kernel_start&&start<&kernel_end&&end>&kernel_end){//partially inside kernel
        start=&kernel_end;//recalculate start address, to be outside kernel space
    }
    if((end-start)<(sizeof(memory_block)*100)){
        return;//invalid, too small
    }else{
        uint8_t * buffer=start;
        start+=sizeof(memory_block)*100;
        build_unused(buffer,100);//reserve amount for 100 memory management blocks
    }
    block_root=get_block(start,end-start);
}

void Memory::init(struct multiboot_info * mbd){
    print(" -Initializing Memory...\n -Loading GDT...\n");
    GDT[0]=encodeGdtEntry(0,0,0);
    GDT[1]=encodeGdtEntry(0,0xFFFFFFFF,0x9A);
    GDT[2]=encodeGdtEntry(0,0xFFFFFFFF,0x92);
    //GDT[3]=encodeGdtEntry(???,sizeof(???),0x89), //???=TSS location, 0x18
    loadgdt((uint32_t)GDT,sizeof(GDT));//load flat 4GB GDT
    print(" -Parsing Memory Map...\n");
    uint64_t total=0;
    uint64_t usable=0;
    memory_block * temp=nullptr;
    if(mbd->flags&MULTIBOOT_INFO_MEM_MAP){
        void * mmap_max=(void*)(mbd->mmap_addr+mbd->mmap_length);
        for(multiboot_memory_map_t * mmap=(multiboot_memory_map_t *)mbd->mmap_addr;mmap<mmap_max;mmap=(multiboot_memory_map_t *)(((uint32_t)mmap)+mmap->size+sizeof(mmap->size))){
            total+=mmap->len;
            if(mmap->type!=MULTIBOOT_MEMORY_AVAILABLE){//ignore non-available memory
                continue;
            }else if(mmap->addr<MM){//don't bother with lower memory
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
            print("> Memory ");
            Screen::setfgcolor(Screen::RED);
            print("FAIL\n");
            k_abort_s("Not Enough Memory");
            return;
        }
    }else{
        print("> Memory ");
        Screen::setfgcolor(Screen::RED);
        print("FAIL\n");
        k_abort_s("Memory Map not available");
        print("\n");
    }
    print("> Memory ");
    Screen::setfgcolor(Screen::LIGHT_GREEN);
    print("OK\n");
    Screen::setfgcolor(Screen::WHITE);
    print(" -Usable Memory: ");
    Screen::setfgcolor(Screen::LIGHT_GREEN);
    Screen::write_mem(usable);
    Screen::setfgcolor(Screen::WHITE);
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

memory_block * Memory::alloc_block(uint32_t size){//first fit
    memory_block * block;
    for(block=block_root;block!=nullptr&&(block->size<size||block->type!=memory_block::MEMORY_BLOCK_FREE);block=block->next);
    if(block==nullptr){
        return nullptr;//out of memory
    }else{
        return split(block,size);
    }
}

void Memory::free_block(memory_block * ptr){
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

void * Memory::kmalloc(uint32_t size){
    if(memory_block * b=alloc_block(size)){
        return b->start;
    }else{
        return nullptr;
    }
}

void Memory::kfree(void * ptr){
    memory_block * block=block_root;
    while(block!=nullptr){
        if(block->start==ptr)break;//ptr found
        else if(ptr<block->start)return;//ptr not valid
    }
    if(block!=nullptr){
        free_block(block);
    }
}
