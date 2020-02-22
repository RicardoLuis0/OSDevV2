#ifndef KMEM_H_INCLUDED
#define KMEM_H_INCLUDED

#include "stdc/stdint.h"

namespace Memory{
    extern const uint32_t page_size;
    void cmd_meminfo();
    void * alloc_phys_page();//get a new physical page (and mark it as used)
    void free_phys_page(void*);//mark physical page as unused
    void * alloc_virt_page();//get a new virtual page (and mark it as used)
    void free_virt_page(void*);//mark virtual page as unused
}

#endif // KMEM_H_INCLUDED
