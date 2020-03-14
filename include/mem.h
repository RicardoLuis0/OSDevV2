#ifndef KMEM_H_INCLUDED
#define KMEM_H_INCLUDED

#include "stdc/stdint.h"
#include "stdc/stddef.h"

namespace Memory{
    void cmd_meminfo();
    constexpr size_t pages_to_fit(size_t size){//calculate enough pages to fit 'size' bytes of data
        return (size%4096)?(size/4096)+1:size/4096;
    }
    namespace Internal {
        bool is_phys_page_free(uint32_t page_id);//check if page with id
        void * phys_id_to_ptr(uint32_t page_id);
        void * alloc_phys_page(uint32_t n);//get 'n' new physical pages (and mark as used)
        void free_phys_page(void*,uint32_t n);//mark 'n' physical pages as unused
    }
    void * alloc_virt_page(uint32_t n);//get 'n' new virtual pages (and mark as used)
    void free_virt_page(void*,uint32_t n);//mark 'n' virtual pages as unused
}

#endif // KMEM_H_INCLUDED
