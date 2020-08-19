#include "arch/x86.h"
#include <stdint.h>
#include "klib.h"


namespace PIC {
    
    void init(){
        //remap PIC
        outb(0x20, 0x11);//initialize PIC1
        outb(0xA0, 0x11);//initialize PIC2
        outb(0x21, 0x20);//set PIC1 vector offset (0-7)->(32-39)
        outb(0xA1, 0x28);//set PIC2 vector offset (8-15)->(40-47)
        outb(0x21, 0x04);//configure master PIC
        outb(0xA1, 0x02);//configure slave PIC
        outb(0x21, 0x01);//set 8086 mode
        outb(0xA1, 0x01);//set 8086 mode
        //mask all interrupts
        outb(0x21, 0xff);
        outb(0xA1, 0xff);
    }
    
    void unmask(uint8_t irq){
        uint8_t port;
        if(irq<8){
            port=0x21;
        }else{
            port=0xA1;
            irq-=8;
        }
        uint8_t data=inb(port)&~(1<<irq);
        outb(port,data);
    }
    
    void mask(uint8_t irq){
        uint8_t port;
        if(irq<8){
            port=0x21;
        }else{
            port=0xA1;
            irq-=8;
        }
        uint8_t data=inb(port)|(1<<irq);
        outb(port,data);
    }
    
}
