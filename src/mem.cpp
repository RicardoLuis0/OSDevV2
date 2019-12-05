#include "extern/multiboot.h"
#include "klib.h"
#include "mem.h"
#include "screen.h"
#include "print.h"

struct gdt_entry{
    uint8_t arr[8];
};

static inline gdt_entry encodeGdtEntry(uint32_t base, uint32_t limit, uint32_t type) {
    //adapted from https://wiki.osdev.org/GDT_Tutorial, accessed on decembeer 5 2019
    // Check the limit to make sure that it can be encoded
    gdt_entry target;
    if ((limit > 65536) && ((limit & 0xFFF) != 0xFFF)) {
        k_abort_s("Invalid limit for GDT entry!");
    }
    if (limit > 65536) {
        // Adjust granularity if required
        limit = limit >> 12;
        target.arr[6] = 0xC0;
    } else {
        target.arr[6] = 0x40;
    }
    // Encode the limit
    target.arr[0] = limit & 0xFF;
    target.arr[1] = (limit >> 8) & 0xFF;
    target.arr[6] |= (limit >> 16) & 0xF;
    // Encode the base 
    target.arr[2] = base & 0xFF;
    target.arr[3] = (base >> 8) & 0xFF;
    target.arr[4] = (base >> 16) & 0xFF;
    target.arr[7] = (base >> 24) & 0xFF;
    // And... Type
    target.arr[5] = type;
    return target;
}

gdt_entry GDT[3];

extern "C" void loadgdt(uint32_t base,uint16_t limit);

extern "C" void * k_malloc(uint32_t size){
    return NULL;
}

void Memory::init(multiboot_info_t * mbd){
    print(" -Initializing Memory...\n  -Loading GDT...\n");
    GDT[0]=encodeGdtEntry(0,0,0);
    GDT[1]=encodeGdtEntry(0,0xFFFFFFFF,0x9A);
    GDT[2]=encodeGdtEntry(0,0xFFFFFFFF,0x92);
    //GDT[3]=encodeGdtEntry(???,sizeof(???),0x89), //???=TSS location, 0x18
    loadgdt((uint32_t)GDT,sizeof(GDT));//load flat 4GB GDT
    print("  -Parsing Memory Map...\n");
    if(mbd->flags&MULTIBOOT_INFO_MEM_MAP){
        //TODO read memory map
    }else{
        print(" > Memory initialization ");
        Screen::setfgcolor(Screen::RED);
        print("FAIL\n");
        k_abort_s("Memory Map not available");
    }
    print(" > Memory initialization ");
    Screen::setfgcolor(Screen::GREEN);
    print("OK\n");
    Screen::setfgcolor(Screen::WHITE);
    k_abort_s("Memory not implemented yet");
}
