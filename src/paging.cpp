#include "paging.h"
#include "klib.h"
#include "print.h"
#include "mem.h"

using namespace Memory;

namespace Memory{
    //assembled using information from https://wiki.osdev.org/Paging
    struct page_entry_t {
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
    }
}

static inline page_entry_t * id_to_page_entry(uint32_t page_id,page_entry_t * pd_a){
    return reinterpret_cast<page_entry_t *>(pd_a[page_id/1024].address<<12)+(page_id%1024);
}

static inline void set_page_table_entry(page_entry_t * pte,page_table_entry_flags_t flags,void * pages){
    uint32_t addr=reinterpret_cast<uint32_t>(pages);
    if(addr%4096){
        k_abort_s("misaligned page table entry");
    }
    pte->address=addr>>12;
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

static inline void set_page_directory_entry(page_entry_t * pde,page_directory_entry_flags_t flags,void * tables){
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
    k_abort_s("map_virtual_page unimplemented");
    //TODO
    return v;
}

void Memory::unmap_virtual_page(uint32_t v,uint32_t n){
    k_abort_s("unmap_virtual_page unimplemented");
    //TODO
}

uint32_t Memory::get_mapping_phys(uint32_t p){
    k_abort_s("get_mapping_phys unimplemented");
    //TODO
}

uint32_t Memory::get_mapping_virt(uint32_t v){
    k_abort_s("get_mapping_virt unimplemented");
    //TODO
}

uint32_t Memory::next_free_virt_page(){
    k_abort_s("next_free_virt_page unimplemented");
    //TODO
}

extern uint8_t kernel_start;

extern uint8_t kernel_end;

void Memory::paging_init(){
    print("\n -Initializing Paging...\n");
    page_entry_t * pd=reinterpret_cast<page_entry_t*>(Internal::alloc_phys_page(pages_to_fit(sizeof(page_entry_t)*1024)));
    for(uint32_t i=0;i<1024;i++){
        page_entry_t * pt=reinterpret_cast<page_entry_t*>(Internal::alloc_phys_page(pages_to_fit(sizeof(page_entry_t)*1024)));
        for(uint32_t j=0;j<1024;j++){
            set_page_table_entry(pt+j,{.present=false,.rw=false,.user=false},nullptr);
        }
        set_page_directory_entry(pd+i,{.present=true,.rw=true,.user=false},pt);
    }
    //TODO setup identity paging for already-allocated pages
    uint32_t i;
    for(i=0;!Internal::is_phys_page_free(i);i++){//map all non-free pages
        set_page_table_entry(id_to_page_entry(i,pd),{.present=true,.rw=true,.user=false},Internal::phys_id_to_ptr(i));
    }
    print("First Free Page:",i);
    
    print("  .Paging ");
    Screen::setfgcolor(Screen::RED);
    print("FAIL");
    Screen::setfgcolor(Screen::WHITE);
    k_abort_s("Paging unimplemented");
}
