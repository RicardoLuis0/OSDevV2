#include "paging.h"
#include "klib.h"
#include "print.h"
#include "mem.h"

using namespace Memory;

namespace Memory{
    //assembled using information from https://wiki.osdev.org/Paging
    struct entry_t {
        uint32_t present        :  1;//is entry present?
        uint32_t rw             :  1;//is entry writable? else is read-only
        uint32_t user           :  1;//does the entry belong to a user? else belongs to the system
        uint32_t write_thru     :  1;//enable write-through caching? else uses write-back caching
        uint32_t cache_disabled :  1;//is caching disabled? else is enabled
        uint32_t accessed       :  1;//was the page accessed since last clear? CPU only sets this bit, must be cleared by the OS if needed
        uint32_t dirty          :  1;//has the page been written to? CPU doesn't modifty this flag, OS must set and clear it if needed, zero for page directory entry
        uint32_t page_size      :  1;//are the pages 4MiB? else 4KiB, needs PSE, zero for page table entry
        uint32_t global         :  1;//???, unused for page directory entry
        uint32_t unused         :  3;
        uint32_t address        : 20;
    };

    struct page_table_entry_flags_t {
        bool present;//is entry present?
        bool rw;//is entry writable? else is read-only
        bool user;//does the entry belong to a user? else belongs to the system
    };

    struct page_directory_entry_flags_t {
        bool present;//is entry present?
        bool rw;//is entry writable? else is read-only
        bool user;//does the entry belong to a user? else belongs to the system
    };

    namespace Internal {
        extern uint64_t total;
        extern uint64_t usable;
        extern uint64_t phys_mem_in_use;
        entry_t * current_page_directory=nullptr;
    }
}

static inline entry_t * id_to_page_entry(uint32_t page_id,entry_t * pd_a){
    constexpr uint32_t last=1<<20;
    if(page_id>=last)k_abort_s("virtual address out of range");
    return reinterpret_cast<entry_t *>(pd_a[page_id/1024].address<<12)+(page_id%1024);
}

static inline void set_page_table_entry(entry_t * pte,page_table_entry_flags_t flags,uint32_t page_id){
    pte->address=page_id;
    pte->present=flags.present;
    pte->rw=flags.rw;
    pte->user=flags.user;
    pte->write_thru=0;
    pte->cache_disabled=0;
    pte->accessed=0;
    pte->dirty=0;
    pte->page_size=0;
    pte->global=0;
    pte->unused=0;
}

static inline void set_page_directory_entry(entry_t * pde,page_directory_entry_flags_t flags,void * tables){
    uint32_t addr=reinterpret_cast<uint32_t>(tables);
    if(addr%4096){
        k_abort_s("misaligned page directory entry");
    }
    pde->address=addr>>12;
    pde->present=flags.present;
    pde->rw=flags.rw;
    pde->user=flags.user;
    pde->write_thru=0;
    pde->cache_disabled=0;
    pde->accessed=0;
    pde->dirty=0;
    pde->page_size=0;
    pde->global=0;
    pde->unused=0;
}

uint32_t Memory::map_virtual_page(uint32_t p,uint32_t v,uint32_t n){
    uint32_t last=v+n;
    for(uint32_t i=v;i<last;i++){
        entry_t * pte=id_to_page_entry(i,Internal::current_page_directory);
        if(!pte->present){
            set_page_table_entry(pte,{.present=true,.rw=true,.user=false},p);
            p++;
        }else{
            k_abort_s("can't map into already-mapped virtual address");
        }
    }
    return v;
}

void Memory::unmap_virtual_page(uint32_t v,uint32_t n){
    uint32_t last=v+n;
    for(uint32_t i=v;i<last;i++){
        entry_t * pte=id_to_page_entry(i,Internal::current_page_directory);
        if(pte->present){
            set_page_table_entry(pte,{.present=false,.rw=false,.user=false},0);
        }else{
            k_abort_s("can't unmap unused virutal address");
        }
    }
}

uint32_t Memory::get_mapping_phys(uint32_t p){
    constexpr uint32_t last=1<<20;
    for(uint32_t i=0;i<last;i++){
        entry_t * pte=id_to_page_entry(i,Internal::current_page_directory);
        if(pte->address==p){
            return i;
        }
    }
    return 0;//can't find address
}

uint32_t Memory::get_mapping_virt(uint32_t v){
    entry_t * pte=id_to_page_entry(v,Internal::current_page_directory);
    if(pte->present){
        return pte->address;
    }
    return 0;//can't find address
}

uint32_t Memory::next_free_virt_page(uint32_t n){
    if(!Internal::current_page_directory) k_abort_s("trying to call Memory::next_free_virt_page while paging is disabled");
    uint32_t c=0;
    for(uint32_t i=0;i<1024;i++){
        entry_t * pt=reinterpret_cast<entry_t*>(Internal::current_page_directory[i].address<<12);
        for(uint32_t j=0;j<1024;j++){
            if(!pt[j].present){
                c++;
                if(c==n)return (i<<10)+j;
            }else{
                c=0;
            }
        }
    }
    k_abort_s("Out of Virtual Memory");
}

extern uint8_t kernel_start;

extern uint8_t kernel_end;

void paging_enable(entry_t * pd){
    asm("movl %0,%%eax\ncall ASM_paging_enable"::"r"(pd));
    Internal::current_page_directory=pd;
}

void Memory::paging_init(){
    print("\n -Initializing Paging...\n");
    entry_t * pd=reinterpret_cast<entry_t*>(Internal::alloc_phys_page(pages_to_fit(sizeof(entry_t)*1024)));
    for(uint32_t i=0;i<1024;i++){
        entry_t * pt=reinterpret_cast<entry_t*>(Internal::alloc_phys_page(pages_to_fit(sizeof(entry_t)*1024)));
        for(uint32_t j=0;j<1024;j++){
            set_page_table_entry(pt+j,{.present=false,.rw=false,.user=false},0);
        }
        set_page_directory_entry(pd+i,{.present=true,.rw=true,.user=false},pt);
    }
    uint32_t i;
    Internal::phys_mem_in_use=0;
    for(i=0;i<=Internal::pages.last_usable;i++){//map all non-free pages
        if(!Internal::is_phys_page_free(i)){
            set_page_table_entry(id_to_page_entry(i,pd),{.present=true,.rw=true,.user=false},i);
            Internal::phys_mem_in_use++;
        }
    }
    Internal::phys_mem_in_use*=4096;
    paging_enable(pd);
    print("  .Paging ");
    Screen::setfgcolor(Screen::LIGHT_GREEN);
    print("OK");
    Screen::setfgcolor(Screen::WHITE);
}
