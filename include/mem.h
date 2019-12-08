#ifndef KMEM_H_INCLUDED
#define KMEM_H_INCLUDED

struct multiboot_info;

namespace Memory{
    struct memory_block{
        uint8_t * start;
        uint32_t size;
        //void * end;//non-inclusive
        memory_block * next;
        memory_block * prev;
        enum memory_block_type{
            MEMORY_BLOCK_FREE,
            MEMORY_BLOCK_USED,
        } type;
    };
    void init(struct multiboot_info * mbd);
    memory_block * alloc_block(uint32_t size);
    void free_block(memory_block * ptr);
}

#endif // KMEM_H_INCLUDED
