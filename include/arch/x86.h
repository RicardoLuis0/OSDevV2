#ifndef X86_H_INCLUDED
#define X86_H_INCLUDED

#include "stdc/stdint.h"
#include "stdc/stddef.h"
#include "extern/multiboot.h"


extern "C" uint32_t has_cpuid();

namespace CPUID {
    void check();
    void cmd_cpuid();
}

namespace Memory{
    void x86_init(multiboot_info * mbd);
    constexpr uint16_t phys_page_segment=32768;
    struct physical_pages_t {
        uint32_t usage[phys_page_segment];//4GB of page usage, each page is 1 bit
        uint32_t last_usable;
    };//THIS IS ~130KB!!!!!!!!!!!!!!!!!!

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
        extern physical_pages_t pages;
        bool is_phys_page_free(uint32_t page_id);//check if page with id
        void * phys_id_to_ptr(uint32_t page_id);
        void * alloc_phys_page(uint32_t n);//get 'n' new physical pages (and mark as used)
        void free_phys_page(void*,uint32_t n);//mark 'n' physical pages as unused
        void set_phys_free(uint32_t page_id_start,uint32_t page_id_end,bool new_free);
        uint32_t count_used_pages();//counts number of non-free physical pages
    }
    void * alloc_virt_page(uint32_t n);//get 'n' new virtual pages (and mark as used)
    void free_virt_page(void*,uint32_t n);//mark 'n' virtual pages as unused
}

namespace Screen{
    void x86_init();
}

namespace GDT {
    void init();
}

namespace IDT{
    enum gate_type {
        G_32_INT=0b1110,
        G_32_TASK=0b0101,
        G_32_TRAP=0b1111,
        //G_16_INT=0b0110,
        //G_16_TRAP=0b0111,
    };
    enum ring_type {
        RING_0,
        RING_1,
        RING_2,
        RING_3,
    };
    void init();
    void set_irq_handler(uint8_t num,void(*)(void),gate_type,ring_type);//ignores data
    void set_irq_handler(uint8_t num,void(*)(uint32_t),gate_type,ring_type);//receives data
    void set_irq_handler(uint8_t num,void(*)(uint32_t,uint32_t),gate_type,ring_type);//receives data and irq number (for multi-irq handlers)
    void set_exception_handler(uint8_t num,void(*)(void),gate_type,ring_type);//ignores data
    void set_exception_handler(uint8_t num,void(*)(uint32_t),gate_type,ring_type);//receives data
    void set_exception_handler(uint8_t num,void(*)(uint32_t,uint32_t),gate_type,ring_type);//receives data and irq number (for multi-irq handlers)
    void pic_enable(uint8_t irq);
    void pic_disable(uint8_t irq);

    template<uint32_t irq>
    void call_interrupt(uint32_t data){//call user-defined interrupt
        asm("int %1\n"
            :
            :"a"(data),
             "i"(irq));
    }
}

#endif // X86_H_INCLUDED
