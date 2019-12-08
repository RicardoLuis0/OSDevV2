#include "klib.h"
#include "print.h"
#include "mem.h"
#include "default/assert.h"
#include "kui.h"

extern "C" void k_abort(){
    Screen::setcolor(Screen::RED,Screen::WHITE);
    print("Kernel Aborted!");
    asm volatile("jmp hang");
}

extern "C" void k_abort_i(int code){
    Screen::setcolor(Screen::RED,Screen::WHITE);
    print("Kernel Aborted! Error code '",code,"'");
    asm volatile("jmp hang");
}

extern "C" void k_abort_s(const char * msg){
    Screen::setcolor(Screen::RED,Screen::WHITE);
    print("Kernel Aborted! ",msg,"");
    asm volatile("jmp hang");
}

extern "C" void k_abort_assert(const char * msg,const char * filename,uint32_t line){
    Screen::setcolor(Screen::RED,Screen::WHITE);
    print("Kernel Aborted! In ",filename,":",line,", Assertion ",msg," failed.");
    asm volatile("jmp hang");
}

extern uint32_t kernel_start;

extern uint32_t kernel_end;

constexpr uint64_t MM=(1024ULL*1024ULL);

extern "C" void k_main(struct multiboot_info * mbd, unsigned int magic){
    Screen::init();
    fassert(magic==0x2BADB002U);
    Screen::write_s(" -Starting Kernel...\n -Kernel Size: ");
    Screen::setfgcolor(Screen::LIGHT_GREEN);
    Screen::write_mem((((uint32_t)&kernel_end)-((uint32_t)&kernel_start))-MM);//size of kernel in memory minus stack size
    Screen::setfgcolor(Screen::WHITE);
    Screen::write_s("\n -Stack Size:  ");
    Screen::setfgcolor(Screen::LIGHT_GREEN);
    Screen::write_mem(1,2);//1M
    Screen::setfgcolor(Screen::WHITE);
    Screen::write_s("\n -Initializing Drivers...\n");
    Memory::init(mbd);
    //KUI::kui();
}
