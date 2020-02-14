#ifndef KMEM_H_INCLUDED
#define KMEM_H_INCLUDED

#include "stdc/stdint.h"

namespace Memory{
    void cmd_meminfo();
    void * alloc_page();
    void free_page(void*);
}

#endif // KMEM_H_INCLUDED
