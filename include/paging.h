#ifndef PAGING_H
#define PAGING_H

#include "stdc/stdint.h"

namespace Memory {
    constexpr uint16_t phys_page_segment=32768;
    struct physical_pages_t {
        uint32_t usage[phys_page_segment];//4GB of page usage, each page is 1 bit
        uint32_t last_usable;
    };//THIS IS ~130KB!!!!!!!!!!!!!!!!!!

    namespace Internal {
        extern physical_pages_t pages;
    }

    uint32_t map_virtual_page(uint32_t p,uint32_t v,uint32_t n);//map 'n' contiguous pages at physicall address 'p' to virtual address 'v'

    void unmap_virtual_page(uint32_t v,uint32_t n);//unmap 'n' virtual pages at virtual address 'v'

    uint32_t get_mapping_phys(uint32_t p);//get first virtual mapping of physical address

    uint32_t get_mapping_virt(uint32_t v);//get physical mapping of virtual address

    uint32_t next_free_virt_page(uint32_t n);//get a free unused virtual address ( to pass to map_virtual_page ) that can fit 'n' pages

    void paging_init();

}

#endif // PAGING_H
