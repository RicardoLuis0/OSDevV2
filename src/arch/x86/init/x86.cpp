#include "arch/x86.h"
#include "screen.h"
#include "klib.h"
#include "stdc/assert.h"
#include "acpi.h"

#include "fs/ramfs.h"

#ifdef DEBUG
static volatile bool global_constructors_ran;
static __attribute__ ((constructor)) void global_constructors_ok(void) {
    global_constructors_ran=true;
}
#endif // DEBUG

extern "C" {
    
    void outb(uint16_t port, uint8_t val) {
        asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
    }
    
    uint8_t inb(uint16_t port) {
        uint8_t ret;
        asm volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
        return ret;
    }
    
    void outw(uint16_t port, uint16_t val){
        asm volatile ( "outw %0, %1" : : "a"(val), "Nd"(port) );
    }
    
    uint16_t inw(uint16_t port){
        uint16_t ret;
        asm volatile ( "inw %1, %0" : "=a"(ret) : "Nd"(port) );
        return ret;
    }
    
    void outl(uint16_t port, uint32_t val){
        asm volatile ( "outl %0, %1" : : "a"(val), "Nd"(port) );
    }
    
    uint32_t inl(uint16_t port){
        uint32_t ret;
        asm volatile ( "inl %1, %0" : "=a"(ret) : "Nd"(port) );
        return ret;
    }
    
}

extern "C" void _init();

bool klib_init();

extern "C" void x86_start(struct multiboot_info * mbd, unsigned int magic){//x86-specific initialization sequence
    Screen::x86_init();
    Screen::disable_cursor();
    fassert(magic==0x2BADB002U);
    if(!CPUID::has_cpuid()){
        k_abort_s("CPUID not supported");
    }
    CPUID::check();
    Screen::write_s(">Initializing x86 Internals");
    GDT::init();
    Memory::x86_init(mbd);//initialize physical memory
    IDT::init();
    Memory::x86_paging_init();//initialize virtual memory
    FPU::init();
    Serial::x86_init();
    PIT::init();
    ACPI::init();
    Screen::write_s("\n>Global Constructors...");
    #ifdef DEBUG
    global_constructors_ran=false;
    #endif // DEBUG
    _init();//only call global constructors after setting up screen, paging, memory management, etc...
    #ifdef DEBUG
    if(global_constructors_ran){
    #endif // DEBUG
        Screen::setfgcolor(Screen::LIGHT_GREEN);
        Screen::write_s("OK");
    #ifdef DEBUG
    }else{
        Screen::setfgcolor(Screen::RED);
        Screen::write_s("FAIL");
        k_abort_s("Global Constructors Failed to Run");
    }
    #endif // DEBUG
    Screen::setfgcolor(Screen::WHITE);
    Screen::write_s("\n>Initializing VFS...");
    FS::VFSRoot::init(new FS::RamFS());
    Screen::setfgcolor(Screen::LIGHT_GREEN);
    Screen::write_s("OK");
    Screen::setfgcolor(Screen::WHITE);
    Screen::write_s("\n>Initializing KLib...");
    if(klib_init()){
        Screen::setfgcolor(Screen::LIGHT_GREEN);
        Screen::write_s("OK");
    }else{
        Screen::setfgcolor(Screen::RED);
        Screen::write_s("FAIL");
        k_abort_s("KLib Initialization Failed");
    }
    Screen::setfgcolor(Screen::WHITE);
    return;
}
