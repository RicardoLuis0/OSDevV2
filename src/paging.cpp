#include "paging.h"
#include "klib.h"
#include "print.h"

using namespace Memory;

namespace Memory::Internal {
    extern uint64_t total;
    extern uint64_t usable;
}

void Memory::make_page_table_entry(page_entry_t * pte,page_table_entry_flags_t flags,void * pages){
    uint32_t addr=reinterpret_cast<uint32_t>(pages);
    if(addr%4096){
        k_abort_s("misaligned page table entry");
    }
    pte->address=addr<<12;
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

void Memory::make_page_directory_entry(page_entry_t * pde,page_directory_entry_flags_t flags,void * tables){
    uint32_t addr=reinterpret_cast<uint32_t>(tables);
    if(addr%4096){
        k_abort_s("misaligned page directory entry");
    }
    pde->address=addr<<12;
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

void Memory::paging_init(){
    print("\n -Initializing Paging...\n");
    print("  .Paging ");
    Screen::setfgcolor(Screen::RED);
    print("FAIL");
    Screen::setfgcolor(Screen::WHITE);
    k_abort_s("Paging unimplemented");
}
