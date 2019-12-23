#include "idt.h"
#include "default/stdint.h"

using namespace IDT;

namespace IDT {
    //https://wiki.osdev.org/Interrupt_Descriptor_Table
    union IDT_type_attributes {
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
        uint8_t raw;
        struct{
            uint8_t type:4;
            uint8_t segment:1;
            uint8_t dpl:2;
            uint8_t present:1;
        };
        IDT_type_attributes(){
            type=0;
            segment=0;
            dpl=0;
            present=0;
        }
        IDT_type_attributes(gate_type t,ring_type priviledge){
            type=t;
            segment=(t==G_32_TASK)?1:0;
            dpl=priviledge;
            present=1;
        }
    };
    struct IDT_entry {
        uint16_t offset_1;
        uint16_t selector;
        uint8_t zero;
        IDT_type_attributes type_attr;
        uint16_t offset_2;
        void encode(uint32_t address,IDT_type_attributes t_a){
            offset_1=address&0xFFFF;
            selector=0x08;
            zero = 0;
            type_attr = t_a;
            offset_2=address>>16;
        }
    };
    IDT_entry IDT[256];
    void (*idt_callback[256])(void);
}

extern "C" void call_idt(int num){
    if(num<0||num>255)return;//too large/small
    if(idt_callback[num]){
        idt_callback[num]();
    }
}

void IDT::init(){
    //TODO
}

void IDT::set_handler(uint8_t num,void(*c)(void)){
    if(num<32){
        return;//can't change reserved callbacks
    }else{
        idt_callback[num]=c;
    }
}
