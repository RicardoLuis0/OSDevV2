#include "arch/x86.h"
#include "stdc/stdint.h"
#include "klib.h"
#include "screen.h"

using namespace GDT;

namespace GDT {
    struct gdt_entry{
        uint8_t arr[8];
        inline void encode(uint32_t base, uint32_t limit, uint32_t type){
            if ((limit > 65536) && ((limit & 0xFFF) != 0xFFF)) {
                k_abort_s("Invalid limit for GDT entry!");
            }
            if (limit > 65536) {
                // Adjust granularity if required
                limit = limit >> 12;
                arr[6] = 0xC0;
            } else {
                arr[6] = 0x40;
            }
            // Encode the limit
            arr[0] = limit & 0xFF;
            arr[1] = (limit >> 8) & 0xFF;
            arr[6] |= (limit >> 16) & 0xF;
            // Encode the base 
            arr[2] = base & 0xFF;
            arr[3] = (base >> 8) & 0xFF;
            arr[4] = (base >> 16) & 0xFF;
            arr[7] = (base >> 24) & 0xFF;
            // And... Type
            arr[5] = type;
        }
    };
    gdt_entry GDT[3];
}
extern "C" void loadgdt(uint32_t base,uint16_t limit);

void GDT::init(){
    Screen::write_s("\n -Loading GDT...");
    GDT[0].encode(0,0,0);
    GDT[1].encode(0,0xFFFFFFFF,0x9A);
    GDT[2].encode(0,0xFFFFFFFF,0x92);
    //GDT[3].encode(???,sizeof(???),0x89), //???=TSS location, 0x18
    loadgdt(reinterpret_cast<uint32_t>(GDT),sizeof(GDT));//load flat 4GB GDT
}
