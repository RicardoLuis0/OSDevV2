#include "arch/x86.h"
#include "klib.h"
#include "print.h"
#include <stdio.h>

using namespace Memory;

namespace Memory{
    //assembled using information from https://wiki.osdev.org/Paging
    struct entry_t {
        uint32_t present        :  1;//is entry present?
        uint32_t rw             :  1;//is entry writable? else is read-only
        uint32_t user           :  1;//does the entry belong to a user? else belongs to the system
        uint32_t write_thru     :  1;//enable write-through caching? else uses write-back caching
        uint32_t cache_disabled :  1;//is caching disabled? else is enabled
        uint32_t accessed       :  1;//was the page accessed since last clear? CPU only sets this bit, must be cleared by the OS if needed
        uint32_t dirty          :  1;//has the page been written to? CPU doesn't modifty this flag, OS must set and clear it if needed, zero for page directory entry
        uint32_t page_size      :  1;//are the pages 4MiB? else 4KiB, needs PSE, zero for page table entry
        uint32_t global         :  1;//???, unused for page directory entry
        uint32_t unused_1       :  1;
        uint32_t unused_2       :  1;
        uint32_t unused_3       :  1;
        uint32_t address        : 20;
    };
    
    struct page_directory_root_t {
        entry_t pd[1024];
        int16_t free[1024];
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
    
    namespace Internal {
        extern uint64_t total;
        extern uint64_t usable;
        extern uint64_t phys_mem_in_use;
        page_directory_root_t * current_page_directory=nullptr;
    }
    
}

static inline entry_t * id_to_page_entry(uint32_t page_id,entry_t * pd_a){
    constexpr uint32_t last=1<<20;
    if(page_id>=last)k_abort_s("virtual address out of range");
    return reinterpret_cast<entry_t *>(pd_a[page_id/1024].address<<12)+(page_id%1024);
}

static inline void set_page_table_entry(entry_t * pte,page_table_entry_flags_t flags,uint32_t page_id){
    pte->address=page_id;
    pte->present=flags.present;
    pte->rw=flags.rw;
    pte->user=flags.user;
    pte->write_thru=0;
    pte->cache_disabled=0;
    pte->accessed=0;
    pte->dirty=0;
    pte->page_size=0;
    pte->global=0;
    pte->unused_1=0;
    pte->unused_2=0;
    pte->unused_3=0;
}

static inline bool set_page_entry_table_used(page_directory_root_t * pd,uint32_t virt_page_id,page_table_entry_flags_t flags,uint32_t phys_page_id){
    auto * pte=id_to_page_entry(virt_page_id,pd->pd);
    flags.present=true;
    if(!pte->present){
        pd->free[virt_page_id/1024]--;
    }else{
        return false;
    }
    set_page_table_entry(pte,flags,phys_page_id);
    return true;
}

static inline bool set_page_entry_table_free(page_directory_root_t * pd,uint32_t virt_page_id){
    auto * pte=id_to_page_entry(virt_page_id,pd->pd);
    if(pte->present){
        pd->free[virt_page_id/1024]++;
    }else{
        return false;
    }
    set_page_table_entry(pte,{.present=false,.rw=false,.user=false},0);
    return true;
}

static inline void set_page_directory_entry(entry_t * pde,page_directory_entry_flags_t flags,void * tables){
    uint32_t addr=reinterpret_cast<uint32_t>(tables);
    if(addr%4096){
        k_abort_s("misaligned page directory entry");
    }
    pde->address=addr>>12;
    pde->present=flags.present;
    pde->rw=flags.rw;
    pde->user=flags.user;
    pde->write_thru=0;
    pde->cache_disabled=0;
    pde->accessed=0;
    pde->dirty=0;
    pde->page_size=0;
    pde->global=0;
    pde->unused_1=0;
    pde->unused_2=0;
    pde->unused_3=0;
}

uint32_t Memory::Internal::map_virtual_page_unsafe(uint32_t p,uint32_t v,uint32_t n,bool allow_remap_if_same){
    if(v==0){
        k_abort_s("can't map invalid virtual address");
    }
    for(uint32_t i=0;i<n;i++){
        if(!set_page_entry_table_used(Internal::current_page_directory,v+i,{.present=true,.rw=true,.user=false},p+i)){
            entry_t * pte=id_to_page_entry(v+i,Internal::current_page_directory->pd);
            if(allow_remap_if_same&&(pte->address==(p+i)))continue;//allow 'remapping' already mapped pages to same address
            k_abort_s("can't map mapped virtual address");
        }
    }
    return v;
}

uint32_t Memory::map_virtual_page(uint32_t p,uint32_t v,uint32_t n){
    if(p>=Internal::pages.last_usable){
        k_abort_s("can't map invalid physical address");
    }
    return Internal::map_virtual_page_unsafe(p,v,n,false);
}

void Memory::unmap_virtual_page(uint32_t v,uint32_t n){
    if(v==0){
        k_abort_s("can't unmap invalid virtual address");
    }
    for(uint32_t i=0;i<n;i++){
        if(!set_page_entry_table_free(Internal::current_page_directory,v+i)){
            k_abort_s("can't unmap unmapped virtual address");
        }
    }
}

void Memory::Internal::pages_for(uint32_t addr,uint32_t len,uint32_t &p_id,uint32_t &n){
    p_id=addr>>12;
    uint32_t start=p_id<<12;
    uint32_t end=addr+len;
    uint32_t len2=end-start;
    n=len2>>12;
    if(len2%4096){
        n++;
    }
}

void Memory::Internal::map_null(){
    set_page_table_entry(id_to_page_entry(0,current_page_directory->pd),{.present=true,.rw=true,.user=true},0);
}

void Memory::Internal::unmap_null(){
    set_page_table_entry(id_to_page_entry(0,current_page_directory->pd),{.present=false,.rw=false,.user=false},0);
}

uint32_t Memory::get_mapping_phys(uint32_t p){
    constexpr uint32_t last=1<<20;
    for(uint32_t i=1;i<last;i++){
        entry_t * pte=id_to_page_entry(i,Internal::current_page_directory->pd);
        if(pte->address==p){
            return i;
        }
    }
    return 0;//can't find address
}

uint32_t Memory::get_mapping_virt(uint32_t v){
    entry_t * pte=id_to_page_entry(v,Internal::current_page_directory->pd);
    if(pte->present){
        return pte->address;
    }
    return 0;//can't find address
}

uint32_t Memory::next_free_virt_page_allow_fail(uint32_t n){
    if(!Internal::current_page_directory) k_abort_s("trying to call Memory::next_free_virt_page while paging is disabled");
    constexpr uint32_t last=1<<20;
    uint32_t c=0;
    uint32_t s=0;
    for(uint32_t i=Internal::current_page_directory->free[0]==0?1024:1;i<last;i++){
        if(i%1024==0){
            while(Internal::current_page_directory->free[i/1024]==0){
                i+=1024;
                c=0;
            }
        }
        entry_t * pte=id_to_page_entry(i,Internal::current_page_directory->pd);
        if(!pte->present){
            if(c==0){
                s=i;
            }
            c++;
            if(c==n)return s;
        }else{
            c=0;
        }
    }
    return 0;
}

uint32_t Memory::next_free_virt_page(uint32_t n){
    uint32_t p=next_free_virt_page_allow_fail(n);
    if(!p) k_abort_s("Out of Virtual Memory");
    return p;
}

extern uint8_t kernel_start;

extern uint8_t kernel_end;

static void paging_enable(page_directory_root_t * pd){
    CR3::set(reinterpret_cast<uint32_t>(pd));
    CR0::enableFlags(CR0::PG);
    Internal::current_page_directory=pd;
}

static void page_fault_handler(uint32_t data){
    Screen::setcolor(Screen::RED,Screen::WHITE);
    Screen::write_s("\nPage Fault trying to access ( ");
    Screen::write_h(CR2::get());
    Screen::write_s(" ): ");
    if(data&0x1){
        Screen::write_s("present ");
    }else{
        Screen::write_s("not-present ");
    }
    if(data&0x2){
        Screen::write_s(", write ");
    }else{
        Screen::write_s(", read ");
    }
    if(data&0x4){
        Screen::write_s(", user ");
    }else{
        Screen::write_s(", kernel ");
    }
    Screen::write_s("( ");
    Screen::write_h(data);
    Screen::write_s(")");
    k_abort();
}

void Memory::cmd_pagefault(){
    uint32_t p=next_free_virt_page(1);
    Screen::write_s("\n Causing Page Fault at (");
    Screen::write_h(p);
    Screen::write_s("), Press Any Key to continue...");
    k_getch_extended();
    *reinterpret_cast<uint32_t*>(p)=0;
}

constexpr size_t entry_arr_pages=pages_to_fit(sizeof(entry_t)*1024u);
constexpr size_t mem_needed=(entry_arr_pages*1024u+pages_to_fit(sizeof(page_directory_root_t)))*4096;

namespace Memory::Internal {
    extern uint64_t free_mem;
}

void Memory::x86_paging_init(){
    print("\n -Initializing Paging...");
    if(Internal::free_mem<mem_needed){
        char buf[80];
        char buf_mnstr[21];
        char buf_mfstr[21];
        k_format_mem_str(buf_mnstr,20,mem_needed,0);
        k_format_mem_str(buf_mfstr,20,Internal::free_mem,0);
        snprintf(buf,80,"Not enough memory for page tables\nNeed %s, have %s",buf_mnstr,buf_mfstr);
        k_abort_s(buf);
    }
    page_directory_root_t * pd=reinterpret_cast<page_directory_root_t*>(Internal::alloc_phys_page(pages_to_fit(sizeof(page_directory_root_t))));
    for(uint32_t i=0;i<1024;i++){
        entry_t * pt=reinterpret_cast<entry_t*>(Internal::alloc_phys_page(entry_arr_pages));
        pd->free[i]=1024;
        for(uint32_t j=0;j<1024;j++){
            set_page_table_entry(pt+j,{.present=false,.rw=false,.user=false},0);
        }
        set_page_directory_entry(pd->pd+i,{.present=true,.rw=true,.user=false},pt);
    }
    Internal::phys_mem_in_use=0;
    for(uint32_t i=0;i<=Internal::pages.last_usable;i++){//map all non-free pages
        if(!Internal::is_phys_page_free(i)){
            set_page_entry_table_used(pd,i,{.present=true,.rw=true,.user=false},i);
            Internal::phys_mem_in_use++;
        }
    }
    Internal::phys_mem_in_use*=4096;
    set_page_table_entry(id_to_page_entry(0,pd->pd),{.present=false,.rw=false,.user=false},0);//mark first page as non-present
    paging_enable(pd);
    IDT::set_exception_handler(14,page_fault_handler,IDT::G_32_INT,IDT::RING_0);//enable page fault handler
    Screen::setfgcolor(Screen::LIGHT_GREEN);
    print("OK");
    Screen::setfgcolor(Screen::WHITE);
}
