#ifndef PAGING_H
#define PAGING_H

#include "stdc/stdint.h"

struct blockdata{
    uint64_t start;
    uint64_t end;
};

namespace Memory {

    uint32_t map_virtual_page(uint32_t p,uint32_t v,uint32_t n);//map 'n' contiguous pages at physicall address 'p' to virtual address 'v'

    void unmap_virtual_page(uint32_t v,uint32_t n);//unmap 'n' virtual pages at virtual address 'v'

    uint32_t get_mapping_phys(uint32_t p);//get virtual mapping of physical address

    uint32_t get_mapping_virt(uint32_t v);//get physical mapping of virtual address

    uint32_t next_free_virt_page();//get a free unused virtual address ( to pass to map_virtual_page )

    void paging_init(blockdata*,uint32_t n);
}

#endif // PAGING_H
