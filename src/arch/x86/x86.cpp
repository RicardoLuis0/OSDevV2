#include "arch/x86.h"
#include "screen.h"
#include "klib.h"
#include "stdc/assert.h"

static volatile bool global_constructors_ran;

static __attribute__ ((constructor)) void global_constructors_ok(void) {
    global_constructors_ran=true;
}

extern "C" void outb(uint16_t port, uint8_t val) {
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

extern "C" uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
    return ret;
}

extern "C" void _init();

extern "C" void x86_start(struct multiboot_info * mbd, unsigned int magic){//x86-specific initialization sequence
    Screen::x86_init();
    Screen::disable_cursor();
    fassert(magic==0x2BADB002U);
    if(!has_cpuid()){
        k_abort_s("CPUID not supported");
    }
    CPUID::check();
    Screen::write_s(">Initializing x86 System");
    GDT::init();
    Memory::x86_init(mbd);//initialize physical memory
    IDT::init();
    Memory::x86_paging_init();//initialize virtual memory
    Screen::write_s("\n>Calling Global Constructors");
    global_constructors_ran=false;
    _init();//only call global constructors after setting up screen, paging, memory management, etc...
    Screen::write_s("\n -Global Constructors ");
    if(global_constructors_ran){
        Screen::setfgcolor(Screen::LIGHT_GREEN);
        Screen::write_s("OK");
    }else{
        Screen::setfgcolor(Screen::RED);
        Screen::write_s("FAIL");
        k_abort_s("Global Constructors Failed to Run");
    }
    Screen::setfgcolor(Screen::WHITE);
    return;
}
