#include "arch/x86.h"
#include "screen.h"
#include "klib.h"
#include "stdc/assert.h"
#include "acpi.h"

#include "fs/ramfs.h"

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
    
    //it's safe to call Screen::write/etc after this point
    
    Screen::disable_cursor();
    
    //assert multiboot 1 magic is correct
    fassert(magic==0x2BADB002U);
    
    Screen::write_s(">Initializing x86 System");
    
    CPUID::init();
    
    if(!CPUID::has_cpuid()){
        k_abort_s("CPUID not supported");
    }
    
    //it's safe to call CPUID functions after this point
    
    if(!CPUID::has(0,CPUID::FEAT_EDX_1_MSR)){
        k_abort_s("MSR not supported");
    }
    
    GDT::init();
    //initialize pmm
    Memory::x86_init(mbd);
    
    //it's only safe to call alloc_phys_page after this point
    
    IDT::setup();
    
    //it's safe to call set_irq_handler/set_exception_handler after this point, but not yet safe to call irq_enable (IDT is set up, but interrupts are not enabled yet)
    
    //initialize vmm
    Memory::x86_paging_init();
    
    //it's safe to call alloc_virt_page/new/malloc after this point, but it's not safe to call alloc_phys_page anymore
    
    Serial::x86_init();
    
    //it's safe to call Serial::write/etc after this point
    
    ACPI::init();
    
    //load IDT and enable interrupts
    IDT::init();
    
    //it's safe to call irq_enable after this point
    
    FPU::init();
    
    //it's safe to use the FPU after this point (and SSE if the CPU supports it)
    
    PIT::init();
    
    ACPI::enable();//enter ACPI mode
    
    _init();//only call global constructors after everything's initialized (x86 init internals should never use global constructors for this reason)
    
    Screen::write_s("\n>Creating RamVFS");
    FS::VFSRoot::init(new FS::RamFS());
    Screen::write_s("\n>Initializing KLib");
    if(!klib_init()){
        k_abort_s("KLib Initialization Failed");
    }
    Screen::setfgcolor(Screen::WHITE);
    return;
}
