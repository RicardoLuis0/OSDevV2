#ifndef KMEM_H_INCLUDED
#define KMEM_H_INCLUDED

#include "stdc/stdint.h"

namespace Memory{
    void cmd_meminfo();
    void * alloc_phys_page(uint32_t n);//get 'n' new physical pages (and mark as used)
    void free_phys_page(void*,uint32_t n);//mark 'n' physical pages as unused
    void * alloc_virt_page(uint32_t n);//get 'n' new virtual pages (and mark as used)
    void free_virt_page(void*,uint32_t n);//mark 'n' virtual pages as unused
}

#endif // KMEM_H_INCLUDED
