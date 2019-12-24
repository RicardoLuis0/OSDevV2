#include "idt.h"
#include "default/stdint.h"

#define IDTX(n) extern "C" void _idtx##n();
#define IDTXi() IDTX(0) IDTX(1) IDTX(2) IDTX(3) IDTX(4) IDTX(5) IDTX(6) IDTX(7) IDTX(8) IDTX(9)
#define IDTXd(n) IDTX(n##0) IDTX(n##1) IDTX(n##2) IDTX(n##3) IDTX(n##4) IDTX(n##5) IDTX(n##6) IDTX(n##7) IDTX(n##8) IDTX(n##9)
#define IDTXdi() IDTXi() IDTXd(1) IDTXd(2) IDTXd(3) IDTXd(4) IDTXd(5) IDTXd(6) IDTXd(7) IDTXd(8) IDTXd(9)
#define IDTXdd(n) IDTXd(n##0) IDTXd(n##1) IDTXd(n##2) IDTXd(n##3) IDTXd(n##4) IDTXd(n##5) IDTXd(n##6) IDTXd(n##7) IDTXd(n##8) IDTXd(n##9)
#define IDTXv(n) IDTX(n##0) IDTX(n##1) IDTX(n##2) IDTX(n##3) IDTX(n##4) IDTX(n##5)
#define IDTXdv(n) IDTXd(n##0) IDTXd(n##1) IDTXd(n##2) IDTXd(n##3) IDTXd(n##4) IDTXv(n##5)
#define IDTXa() IDTXdi() IDTXdd(1) IDTXdv(2)

#define listIDTX(n) _idtx##n,
#define listIDTXi() listIDTX(0) listIDTX(1) listIDTX(2) listIDTX(3) listIDTX(4) listIDTX(5) listIDTX(6) listIDTX(7) listIDTX(8) listIDTX(9)
#define listIDTXd(n) listIDTX(n##0) listIDTX(n##1) listIDTX(n##2) listIDTX(n##3) listIDTX(n##4) listIDTX(n##5) listIDTX(n##6) listIDTX(n##7) listIDTX(n##8) listIDTX(n##9)
#define listIDTXdi() listIDTXi() listIDTXd(1) listIDTXd(2) listIDTXd(3) listIDTXd(4) listIDTXd(5) listIDTXd(6) listIDTXd(7) listIDTXd(8) listIDTXd(9)
#define listIDTXdd(n) listIDTXd(n##0) listIDTXd(n##1) listIDTXd(n##2) listIDTXd(n##3) listIDTXd(n##4) listIDTXd(n##5) listIDTXd(n##6) listIDTXd(n##7) listIDTXd(n##8) listIDTXd(n##9)
#define listIDTXv(n) listIDTX(n##0) listIDTX(n##1) listIDTX(n##2) listIDTX(n##3) listIDTX(n##4) listIDTX(n##5)
#define listIDTXdv(n) listIDTXd(n##0) listIDTXd(n##1) listIDTXd(n##2) listIDTXd(n##3) listIDTXd(n##4) listIDTXv(n##5)
#define listIDTXa() listIDTXdi() listIDTXdd(1) listIDTXdv(2)

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

IDTXa()

constexpr void(*idtx[256])(){//array of asm idt entrypoints
    listIDTXa()
};

void IDT::init(){
    //TODO
}

void IDT::set_handler(uint8_t num,void(*c)(void)){
    if(num<32)return;
    idt_callback[num]=c;
}
