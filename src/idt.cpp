#include "idt.h"
#include "stdc/stdint.h"
#include "stdc/assert.h"
#include "klib.h"
#include "screen.h"

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
        uint8_t raw;
        struct{
            uint8_t type:4;
            uint8_t segment:1;
            uint8_t dpl:2;
            uint8_t present:1;
        };
        IDT_type_attributes(){
            type=G_32_INT;
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
    struct idt_callback_t {
        enum{
            IC_NONE,
            IC_V,
            IC_I,
            IC_II,
        } type=IC_NONE;
        union {
            void (*cv)(void);//no args
            void (*ci)(int);//data
            void (*cii)(int,int);//irq,data
        };
    };
    idt_callback_t idt_callback[256]={};
}

extern "C" void call_idt(int irq,int data){
    if(irq<0||irq>255)return;//too large/small
    switch(idt_callback[irq].type){
    case IDT::idt_callback_t::IC_NONE:
        k_abort_s_i_s("Unimplemented Interrupt Handler ",irq," Called");
    case IDT::idt_callback_t::IC_V:
        idt_callback[irq].cv();
        break;
    case IDT::idt_callback_t::IC_I:
        idt_callback[irq].ci(data);
        break;
    case IDT::idt_callback_t::IC_II:
        idt_callback[irq].cii(irq,data);
        break;
    }
    if(irq>=8)outb(0xA0, 0x20);//EOI slave
    outb(0x20, 0x20);//EOI master
}

IDTXa()

constexpr void(*idtx[256])(){//array of asm idt entrypoints
    listIDTXa()
};

extern "C" void loadidt(uint32_t base,uint16_t limit);

void IDT::init(){
    Screen::write_s(" -Loading IDT...\n");
    int cr0;
    asm volatile ("movl %%cr0,%0":"=r"(cr0));
    cr0|=1<<5;
    asm volatile ("movl %0,%%cr0"::"r"(cr0));
    for(uint32_t i=0;i<256;i++){//setup interrupt table
        IDT_type_attributes ta;
        if(i<32){
            if(i==15||(i>=21&&i<=29)||i==31){//if is reserved do nothing
                ta=IDT_type_attributes();
            }else if(i==1||i==3||i==4){
                ta=IDT_type_attributes(G_32_TRAP,RING_0);
            }else{
                ta=IDT_type_attributes(G_32_INT,RING_0);
            }
        }else{
            ta=IDT_type_attributes();
        }
        IDT[i].encode((uint32_t)idtx[i],ta);//encode address
    }
    //remap PIC
    outb(0x20, 0x11);//init PIC1
    outb(0xA0, 0x11);//init PIC2
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);
    outb(0xA1, 0x0);
    //Load IDT
    loadidt((uint32_t)IDT,sizeof(IDT));
}

void IDT::set_irq_handler(uint8_t irq,void(*c)(void),gate_type g,ring_type t){
    fmassert(irq>31,"Cannot map IRQ Handler into Exception area");
    idt_callback[irq].type=idt_callback_t::IC_V;
    idt_callback[irq].cv=c;
    IDT[irq].type_attr=IDT_type_attributes(g,t);
}

void IDT::set_irq_handler(uint8_t irq,void(*c)(int),gate_type g,ring_type t){
    fmassert(irq>31,"Cannot map IRQ Handler into Exception area");
    idt_callback[irq].type=idt_callback_t::IC_I;
    idt_callback[irq].ci=c;
    IDT[irq].type_attr=IDT_type_attributes(g,t);
}

void IDT::set_irq_handler(uint8_t irq,void(*c)(int,int),gate_type g,ring_type t){
    fmassert(irq>31,"Cannot map IRQ Handler into Exception area");
    idt_callback[irq].type=idt_callback_t::IC_II;
    idt_callback[irq].cii=c;
    IDT[irq].type_attr=IDT_type_attributes(g,t);
}

void IDT::set_exception_handler(uint8_t irq,void(*c)(void),gate_type g,ring_type t){
    fmassert(irq<32,"Cannot map Exception Handler into IRQ area");
    idt_callback[irq].type=idt_callback_t::IC_V;
    idt_callback[irq].cv=c;
    IDT[irq].type_attr=IDT_type_attributes(g,t);
}

void IDT::set_exception_handler(uint8_t irq,void(*c)(int),gate_type g,ring_type t){
    fmassert(irq<32,"Cannot map Exception Handler into IRQ area");
    idt_callback[irq].type=idt_callback_t::IC_I;
    idt_callback[irq].ci=c;
    IDT[irq].type_attr=IDT_type_attributes(g,t);
}

void IDT::set_exception_handler(uint8_t irq,void(*c)(int,int),gate_type g,ring_type t){
    fmassert(irq<32,"Cannot map Exception Handler into IRQ area");
    idt_callback[irq].type=idt_callback_t::IC_II;
    idt_callback[irq].cii=c;
    IDT[irq].type_attr=IDT_type_attributes(g,t);
}
