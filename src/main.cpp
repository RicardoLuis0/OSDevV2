#include "klib.h"
#include "print.h"
#include "mem.h"
#include "stdc/assert.h"
#include "drivers/keyboard/ps2/keyboard.h"
#include "drivers/ahci.h"
#include "cpuid.h"
#include "gdt.h"
#include "idt.h"

#include "kshell/kshell.h"

#include "util/shared_ptr.h"
#include "util/hash_table.h"

extern "C" void outb(uint16_t port, uint8_t val) {
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

extern "C" uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
    return ret;
}

extern "C" void k_abort(){
    Screen::setcolor(Screen::RED,Screen::WHITE);
    print("\nKernel Aborted!");
    asm volatile("jmp hang");
    while(true);
}

extern "C" void k_abort_i(int code){
    Screen::setcolor(Screen::RED,Screen::WHITE);
    print("\nKernel Aborted! Error code '",code,"'");
    asm volatile("jmp hang");
    while(true);
}

extern "C" void k_abort_s(const char * msg){
    Screen::setcolor(Screen::RED,Screen::WHITE);
    print("\nKernel Aborted! ",msg,"");
    asm volatile("jmp hang");
    while(true);
}

extern "C" void k_abort_s_i_s(const char * s1,int i,const char * s2){
    Screen::setcolor(Screen::RED,Screen::WHITE);
    print("\nKernel Aborted! ",s1,i,s2,"");
    asm volatile("jmp hang");
    while(true);
}

extern "C" void k_abort_assert(const char * condition,const char * name,uint32_t line){
    Screen::setcolor(Screen::RED,Screen::WHITE);
    print("\nKernel Aborted! In ",name,":",line,", Assertion ",condition," failed.");
    asm volatile("jmp hang");
    while(true);
}

extern "C" void k_abort_massert(const char * condition,const char * msg,const char * name,uint32_t line){
    Screen::setcolor(Screen::RED,Screen::WHITE);
    print("\nKernel Aborted! In ",name,":",line,", Assertion ",condition," failed.\n",msg);
    asm volatile("jmp hang");
    while(true);
}

extern uint32_t kernel_start;

extern uint32_t kernel_end;

constexpr uint32_t STACK_SIZE=32*(1024ULL);
//constexpr uint32_t STACK_SIZE=(1024ULL*1024ULL);

extern "C" void k_main(struct multiboot_info * mbd, unsigned int magic){
    Screen::init();
    fassert(magic==0x2BADB002U);
    Screen::write_s(">Initializing Kernel\n -Kernel Size: ");
    Screen::setfgcolor(Screen::LIGHT_GREEN);
    Screen::write_mem((((uint32_t)&kernel_end)-((uint32_t)&kernel_start))-STACK_SIZE);//size of kernel in memory minus stack size
    Screen::setfgcolor(Screen::WHITE);
    if(!has_cpuid()){
        k_abort_s("CPUID not supported");
    }
    CPUID::check();
    Screen::write_s("\n -Stack Size:  ");
    Screen::setfgcolor(Screen::LIGHT_GREEN);
    Screen::write_mem(STACK_SIZE);//32K
    Screen::setfgcolor(Screen::WHITE);
    GDT::init();
    IDT::init();
    Memory::init(mbd);
    Screen::write_s("\n>Kernel ");
    Screen::setfgcolor(Screen::LIGHT_GREEN);
    Screen::write_s("OK");
    Screen::setfgcolor(Screen::WHITE);
    Screen::write_s("\n>Initializing Drivers");
    Drivers::Keyboard::PS2::init();
    //TODO ATA/IDE driver
    //TODO AHCI driver
    //TODO VGA driver
    Screen::write_s("\n>Drivers ");
    Screen::setfgcolor(Screen::LIGHT_GREEN);
    Screen::write_s("OK");
    Screen::setfgcolor(Screen::WHITE);
    Screen::write_s("\n>Loading Kernel Shell");
    kshell();
    k_abort_s("Kernel Shell Returned!");
}
