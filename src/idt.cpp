#include "idt.h"
#include "default/stdint.h"

using namespace IDT;
namespace IDT {
    struct IDT_entry {
        uint16_t offset_1; // offset bits 0..15
        uint16_t selector; // a code segment selector in GDT or LDT
        uint8_t zero;      // unused, set to 0
        uint8_t type_attr; // type and attributes, see below
        uint16_t offset_2; // offset bits 16..31
        void encode(uint32_t address,uint8_t t_a){
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

extern "C" void call_idt(uint8_t num){
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
