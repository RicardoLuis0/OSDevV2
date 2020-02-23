#include "paging.h"
#include "klib.h"
#include "print.h"

using namespace Memory;

namespace Memory::Internal {
    extern uint64_t total;
    extern uint64_t usable;
    extern page_t pages;
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
