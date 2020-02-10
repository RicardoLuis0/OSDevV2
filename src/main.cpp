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

[[noreturn]] static void k_abort_main(){
    Screen::disable_cursor();
    asm volatile("jmp hang");
    while(true);
}

extern "C" void k_abort(){
    Screen::setcolor(Screen::RED,Screen::WHITE);
    print("\nKernel Aborted!");
    k_abort_main();
}

extern "C" void k_abort_fullscreen(){
    Screen::setcolor(Screen::BLACK,Screen::WHITE);
    Screen::clear();
    Screen::setcolor(Screen::RED,Screen::WHITE);
    Screen::clear_line(0);
    Screen::move(32,0);
    print("Kernel Aborted!");
    k_abort_main();
}

extern "C" void k_abort_i(int code){
    Screen::setcolor(Screen::RED,Screen::WHITE);
    print("\nKernel Aborted! Error code '",code,"'");
    k_abort_main();
}

extern "C" void k_abort_s(const char * msg){
    Screen::setcolor(Screen::RED,Screen::WHITE);
    print("\nKernel Aborted! ",msg,"");
    k_abort_main();
}

extern "C" void k_abort_s_i_s(const char * s1,int i,const char * s2){
    Screen::setcolor(Screen::RED,Screen::WHITE);
    print("\nKernel Aborted! ",s1,i,s2,"");
    k_abort_main();
}

extern "C" void k_abort_assert(const char * condition,const char * name,uint32_t line){
    Screen::setcolor(Screen::RED,Screen::WHITE);
    print("\nKernel Aborted! In ",name,":",line,", Assertion ",condition," failed.");
    k_abort_main();
}

extern "C" void k_abort_massert(const char * condition,const char * msg,const char * name,uint32_t line){
    Screen::setcolor(Screen::RED,Screen::WHITE);
    print("\nKernel Aborted! In ",name,":",line,", Assertion ",condition," failed.\n",msg);
    k_abort_main();
}

extern uint32_t kernel_start;

extern uint32_t kernel_end;

constexpr uint32_t STACK_SIZE=32*(1024ULL);

extern "C" void _init();

extern "C" void k_main(struct multiboot_info * mbd, unsigned int magic){
    Screen::init();
    Screen::disable_cursor();
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
    Screen::write_s("\n>Calling Global Constructors");
    global_constructors_ran=false;
    _init();//only call global constructors after setting up screen and memory
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
    Screen::write_s("\n>Initializing Drivers");
    Drivers::Keyboard::PS2::init();
    //TODO PATA/IDE driver
    //TODO SATA/AHCI driver
    //TODO VGA driver
    Screen::write_s("\n>Drivers ");
    Screen::setfgcolor(Screen::LIGHT_GREEN);
    Screen::write_s("OK");
    Screen::setfgcolor(Screen::WHITE);
    Screen::write_s("\n>Loading Kernel Shell");
    kshell();
    k_abort_s("Kernel Shell Returned!");
}
