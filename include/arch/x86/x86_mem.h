#ifndef X86_MEM_H_INCLUDED
#define X86_MEM_H_INCLUDED

#include <stdint.h>
#include <stddef.h>

namespace Memory{
    
    void cmd_pagefault();
    void x86_init(multiboot_info * mbd);
    constexpr uint16_t phys_page_segment=32768;
    struct physical_pages_t {
        uint32_t usage[phys_page_segment];//4GB of page usage, each page is 1 bit
        uint32_t last_usable;
    };
    
    uint32_t map_virtual_page(uint32_t p,uint32_t v,uint32_t n);//map 'n' contiguous pages at physicall address 'p' to virtual address 'v'
    
    void unmap_virtual_page(uint32_t v,uint32_t n);//unmap 'n' virtual pages at virtual address 'v'
    
    uint32_t get_mapping_phys(uint32_t p);//get first virtual mapping of physical address
    
    uint32_t get_mapping_virt(uint32_t v);//get physical mapping of virtual address
    
    uint32_t next_free_virt_page(uint32_t n);//get a free unused virtual address ( to pass to map_virtual_page ) that can fit 'n' pages
    
    void x86_paging_init();
    
    constexpr size_t pages_to_fit(size_t size){//calculate enough pages to fit 'size' bytes of data
        return (size%4096)?(size/4096)+1:size/4096;
    }
    
    namespace Internal {
        void pages_for(uint32_t addr,uint32_t len,uint32_t &p_id,uint32_t &n);
        extern physical_pages_t pages;
        bool is_phys_page_free(uint32_t page_id);//check if page with id
        void * phys_id_to_ptr(uint32_t page_id);
        void * alloc_phys_page(uint32_t n);//get 'n' new physical pages (and mark as used)
        void free_phys_page(void*,uint32_t n);//mark 'n' physical pages as unused
        void set_phys_free(uint32_t page_id_start,uint32_t page_id_end,bool new_free);
        uint32_t count_used_pages();//counts number of non-free physical pages
        uint32_t map_virtual_page_unsafe(uint32_t p,uint32_t v,uint32_t n,bool allow_remap_if_same);
        
        //map and unmap nullpointer as identity for working with raw data near it
        void map_null();
        void unmap_null();
    }
    void * alloc_virt_page(uint32_t n);//get 'n' new virtual pages (and mark as used)
    void free_virt_page(void*,uint32_t n);//mark 'n' virtual pages as unused
    
}

#endif // X86_MEM_H_INCLUDED
