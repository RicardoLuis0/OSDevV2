#include "arch/x86.h"
#include <stdint.h>
#include <assert.h>
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

#define listIDTX(n) ( _idtx##n ),
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
    struct IDT_type_attributes {
        uint8_t type:4;
        uint8_t segment:1;
        uint8_t dpl:2;
        uint8_t present:1;
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
    static_assert(sizeof(IDT_entry)==8);
    IDT_entry * IDT;
    struct idt_callback_t {
        enum{
            IC_NONE,
            IC_V,
            IC_R,
            IC_I,
            IC_IR,
            IC_II,
            IC_IIR,
        } type=IC_NONE;
        union {
            void (*cv)(void);//no args
            void (*cr)(regs*);//regs
            void (*ci)(uint32_t);//data
            void (*cir)(uint32_t,regs*);//data,regs
            void (*cii)(uint32_t,uint32_t);//irq,data
            void (*ciir)(uint32_t,uint32_t,regs*);//irq,data,regs
        };
    };
    idt_callback_t idt_callback[256]={};
}

bool use_apic;

extern "C" void handle_irq(
                            uint32_t irq,uint32_t data,
                            uint32_t eip,
                            uint32_t edi,uint32_t esi,
                            uint32_t ebp,uint32_t esp,
                            uint32_t ebx,uint32_t edx,
                            uint32_t ecx,uint32_t eax
                          ){
    if(irq<0||irq>255)return;//too large/small
    
    IDT::regs regs{eax,ecx,edx,ebx,esp,ebp,esi,edi,eip};
    
    switch(idt_callback[irq].type){
    case IDT::idt_callback_t::IC_NONE:
        k_abort_s_i_s("Unimplemented Interrupt Handler ",irq," Called");
    case IDT::idt_callback_t::IC_V:
        idt_callback[irq].cv();
        break;
    case IDT::idt_callback_t::IC_R:
        idt_callback[irq].cr(&regs);
        break;
    case IDT::idt_callback_t::IC_I:
        idt_callback[irq].ci(data);
        break;
    case IDT::idt_callback_t::IC_IR:
        idt_callback[irq].cir(data,&regs);
        break;
    case IDT::idt_callback_t::IC_II:
        idt_callback[irq].cii(irq,data);
        break;
    case IDT::idt_callback_t::IC_IIR:
        idt_callback[irq].ciir(irq,data,&regs);
        break;
    }
    if(irq==255){
        outb(0x20, 0x20);
    }
    IDT::eoi();
    
}

IDTXa()

using idth=void(*)();

constexpr idth idtx[256]{//array of asm idt entrypoints
    listIDTXa()
};


extern "C" void loadidt(uint32_t base,uint16_t limit);

atomic_size_t int_en_count=0;

void IDT::enable_interrupts(){
    if(int_en_count==1){
        asm volatile("sti");
    }
    if(int_en_count>0){//prevent negative overflow
        int_en_count--;
    }
}

void IDT::disable_interrupts(){
    if(int_en_count==0){
        asm volatile("cli");
    }
    int_en_count++;
}

atomic_size_t nmi_en_count=0;

void IDT::enable_nmi(){
    if(nmi_en_count==1){
        outb(0x70,inb(0x70)&0x7F);
    }
    if(nmi_en_count>0){//prevent negative overflow
        nmi_en_count--;
    }
}

void IDT::disable_nmi(){
    if(nmi_en_count==0){
        outb(0x70,inb(0x70)|0x80);
    }
    nmi_en_count++;
}

void IDT::setup(){
    Screen::write_s("\n -Creating IDT...");
    IDT=reinterpret_cast<IDT_entry*>(Memory::Internal::alloc_phys_page(Memory::pages_to_fit(sizeof(IDT_entry)*256)));
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
        IDT[i].encode(reinterpret_cast<uint32_t>(idtx[i]),ta);//encode address
    }
    Screen::setfgcolor(Screen::LIGHT_GREEN);
    Screen::write_s("OK");
    Screen::setfgcolor(Screen::WHITE);
}

static void gpf_handler(uint32_t err,regs * regs){
    //APIC::isr_irr_dump();
    k_abort_fmt("GPF, err=%x, a=%x, c=%x, d=%x, b=%x, sp=%x, bp=%x, si=%x, di=%x, ip=%x",err,regs->eax,regs->ecx,regs->edx,regs->ebx,regs->esp,regs->ebp,regs->esi,regs->edi,regs->eip);
}

static void spurious_irq(){
}

void IDT::init(){
    Screen::write_s("\n -Loading IDT...");
    loadidt(reinterpret_cast<uint32_t>(IDT),sizeof(IDT_entry)*256);
    Screen::setfgcolor(Screen::LIGHT_GREEN);
    Screen::write_s("OK");
    Screen::setfgcolor(Screen::WHITE);
    if((use_apic=APIC::supported())){
        APIC::init();
    }else{
        PIC::init();
    }
    set_exception_handler(13,gpf_handler,IDT::G_32_INT,IDT::RING_0);
    
    set_irq_handler(247,spurious_irq,IDT::G_32_INT,IDT::RING_0);
    set_irq_handler(255,spurious_irq,IDT::G_32_INT,IDT::RING_0);
    
    Screen::write_s("\n -Enabling Interrupts...");
    asm volatile("sti");//enable interrupts
    Screen::setfgcolor(Screen::LIGHT_GREEN);
    Screen::write_s("OK");
    Screen::setfgcolor(Screen::WHITE);
}

bool IDT::is_legacy_mode(){
    return !use_apic;
}

void IDT::irq_enable(uint8_t irq){
    if(use_apic){
        APIC::enable(irq);
    }else{
        PIC::unmask(irq);
    }
}

void IDT::eoi(){
    if(use_apic){
        APIC::eoi();
    }else{
        PIC::eoi();
    }
}

void IDT::irq_disable(uint8_t irq){
    if(use_apic){
        APIC::disable(irq);
    }else{
        PIC::mask(irq);
    }
}

uint8_t IDT::irq_get_mapping(uint8_t irq){
    if(use_apic){
        return APIC::get_mapping(irq);
    }else{
        return PIC::get_mapping(irq);
    }
}

void IDT::irq_remap(uint8_t from,uint8_t to){
    if(!use_apic) k_abort_s("interrupt re-remapping not supported for Legacy PIC");
    APIC::remap(from,to);
}

bool IDT::irq_supports_remapping(){
    return use_apic;
}

bool IDT::irq_supports_remapping(uint8_t irq){
    return use_apic&&APIC::has_remapping(irq);
}

void IDT::set_irq_handler(uint8_t irq,void(*c)(void),gate_type g,ring_type t){
    fmassert(irq>31,"Cannot map IRQ Handler into Exception area");
    idt_callback[irq].type=idt_callback_t::IC_V;
    idt_callback[irq].cv=c;
    IDT[irq].encode(reinterpret_cast<uint32_t>(idtx[irq]),IDT_type_attributes(g,t));
}

void IDT::set_irq_handler(uint8_t irq,void(*c)(regs*),gate_type g,ring_type t){
    fmassert(irq>31,"Cannot map IRQ Handler into Exception area");
    idt_callback[irq].type=idt_callback_t::IC_R;
    idt_callback[irq].cr=c;
    IDT[irq].encode(reinterpret_cast<uint32_t>(idtx[irq]),IDT_type_attributes(g,t));
}

void IDT::set_irq_handler(uint8_t irq,void(*c)(uint32_t),gate_type g,ring_type t){
    fmassert(irq>31,"Cannot map IRQ Handler into Exception area");
    idt_callback[irq].type=idt_callback_t::IC_I;
    idt_callback[irq].ci=c;
    IDT[irq].encode(reinterpret_cast<uint32_t>(idtx[irq]),IDT_type_attributes(g,t));
}

void IDT::set_irq_handler(uint8_t irq,void(*c)(uint32_t,regs*),gate_type g,ring_type t){
    fmassert(irq>31,"Cannot map IRQ Handler into Exception area");
    idt_callback[irq].type=idt_callback_t::IC_IR;
    idt_callback[irq].cir=c;
    IDT[irq].encode(reinterpret_cast<uint32_t>(idtx[irq]),IDT_type_attributes(g,t));
}

void IDT::set_irq_handler(uint8_t irq,void(*c)(uint32_t,uint32_t),gate_type g,ring_type t){
    fmassert(irq>31,"Cannot map IRQ Handler into Exception area");
    idt_callback[irq].type=idt_callback_t::IC_II;
    idt_callback[irq].cii=c;
    IDT[irq].encode(reinterpret_cast<uint32_t>(idtx[irq]),IDT_type_attributes(g,t));
}

void IDT::set_irq_handler(uint8_t irq,void(*c)(uint32_t,uint32_t,regs*),gate_type g,ring_type t){
    fmassert(irq>31,"Cannot map IRQ Handler into Exception area");
    idt_callback[irq].type=idt_callback_t::IC_IIR;
    idt_callback[irq].ciir=c;
    IDT[irq].encode(reinterpret_cast<uint32_t>(idtx[irq]),IDT_type_attributes(g,t));
}

void IDT::set_exception_handler(uint8_t irq,void(*c)(void),gate_type g,ring_type t){
    fmassert(irq<32,"Cannot map Exception Handler into IRQ area");
    idt_callback[irq].type=idt_callback_t::IC_V;
    idt_callback[irq].cv=c;
    IDT[irq].encode(reinterpret_cast<uint32_t>(idtx[irq]),IDT_type_attributes(g,t));
}

void IDT::set_exception_handler(uint8_t irq,void(*c)(regs*),gate_type g,ring_type t){
    fmassert(irq<32,"Cannot map Exception Handler into IRQ area");
    idt_callback[irq].type=idt_callback_t::IC_R;
    idt_callback[irq].cr=c;
    IDT[irq].encode(reinterpret_cast<uint32_t>(idtx[irq]),IDT_type_attributes(g,t));
}

void IDT::set_exception_handler(uint8_t irq,void(*c)(uint32_t),gate_type g,ring_type t){
    fmassert(irq<32,"Cannot map Exception Handler into IRQ area");
    idt_callback[irq].type=idt_callback_t::IC_I;
    idt_callback[irq].ci=c;
    IDT[irq].encode(reinterpret_cast<uint32_t>(idtx[irq]),IDT_type_attributes(g,t));
}

void IDT::set_exception_handler(uint8_t irq,void(*c)(uint32_t,regs*),gate_type g,ring_type t){
    fmassert(irq<32,"Cannot map Exception Handler into IRQ area");
    idt_callback[irq].type=idt_callback_t::IC_IR;
    idt_callback[irq].cir=c;
    IDT[irq].encode(reinterpret_cast<uint32_t>(idtx[irq]),IDT_type_attributes(g,t));
}

void IDT::set_exception_handler(uint8_t irq,void(*c)(uint32_t,uint32_t),gate_type g,ring_type t){
    fmassert(irq<32,"Cannot map Exception Handler into IRQ area");
    idt_callback[irq].type=idt_callback_t::IC_II;
    idt_callback[irq].cii=c;
    IDT[irq].encode(reinterpret_cast<uint32_t>(idtx[irq]),IDT_type_attributes(g,t));
}

void IDT::set_exception_handler(uint8_t irq,void(*c)(uint32_t,uint32_t,regs*),gate_type g,ring_type t){
    fmassert(irq<32,"Cannot map Exception Handler into IRQ area");
    idt_callback[irq].type=idt_callback_t::IC_IIR;
    idt_callback[irq].ciir=c;
    IDT[irq].encode(reinterpret_cast<uint32_t>(idtx[irq]),IDT_type_attributes(g,t));
}

void IDT::set_raw_irq_handler(uint8_t irq,void * h,gate_type g,ring_type t){
    fmassert(irq>31,"Cannot map IRQ Handler into Exception area");
    IDT[irq].encode(reinterpret_cast<uint32_t>(h),IDT_type_attributes(g,t));
}

void IDT::set_raw_exception_handler(uint8_t irq,void * h,gate_type g,ring_type t){
    fmassert(irq<32,"Cannot map Exception Handler into IRQ area");
    IDT[irq].encode(reinterpret_cast<uint32_t>(h),IDT_type_attributes(g,t));
}

void IDT::disable_irq_handler(uint8_t irq){
    fmassert(irq<32,"Handler is not IRQ, is Exception");
    IDT[irq].type_attr.present=0;
}

void IDT::disable_exception_handler(uint8_t irq){
    fmassert(irq<32,"Handler is not Exception, is IRQ");
    IDT[irq].type_attr.present=0;
}


