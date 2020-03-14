#ifndef PAGING_H
#define PAGING_H

#include "stdc/stdint.h"

namespace Memory {

    struct physical_pages_t {
        uint32_t usage[4096];//4GB of page usage, each page is 1 bit
    };

    namespace Internal {
        extern physical_pages_t pages;
    }

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

    void make_page_table_entry(page_entry_t*,page_table_entry_flags_t flags,void * pages);

    void make_page_directory_entry(page_entry_t*,page_directory_entry_flags_t flags,void * tables);

    uint32_t map_virtual_page(uint32_t p,uint32_t v,uint32_t n);//map 'n' contiguous pages at physicall address 'p' to virtual address 'v'

    void unmap_virtual_page(uint32_t v,uint32_t n);//unmap 'n' virtual pages at virtual address 'v'

    uint32_t get_mapping_phys(uint32_t p);//get virtual mapping of physical address

    uint32_t get_mapping_virt(uint32_t v);//get physical mapping of virtual address

    uint32_t next_free_virt_page();//get a free unused virtual address ( to pass to map_virtual_page )

    void paging_init();

}

#endif // PAGING_H
