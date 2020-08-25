#include "arch/x86.h"
#include <stdint.h>
#include "klib.h"
#include "screen.h"


namespace PIC {
    
    void reset(uint8_t off1,uint8_t off2){
        outb(0x20, 0x11);//initialize PIC1
        outb(0xA0, 0x11);//initialize PIC2
        outb(0x21, off1);//set PIC1 vector offset
        outb(0xA1, off2);//set PIC2 vector offset
        outb(0x21, 0x04);//configure master PIC
        outb(0xA1, 0x02);//configure slave PIC
        outb(0x21, 0x01);//set 8086 mode
        outb(0xA1, 0x01);//set 8086 mode
        //mask all interrupts
        outb(0x21, 0xff);
        outb(0xA1, 0xff);
        
    }
    
    void init(){
        Screen::write_s("\n -Initializing PIC...");
        //remap PIC
        reset(0x20,0x28);
        Screen::setfgcolor(Screen::LIGHT_GREEN);
        Screen::write_s("OK");
        Screen::setfgcolor(Screen::WHITE);
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
    
    uint8_t get_mapping(uint8_t irq){
        return (irq<16)?irq+32:irq;
    }
    
}
