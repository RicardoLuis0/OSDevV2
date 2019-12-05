#include "extern/multiboot.h"
#include "klib.h"
#include "print.h"
#include "mem.h"

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

extern "C" void k_main(multiboot_info_t* mbd, unsigned int magic){
    Screen::init();
    Memory::init(mbd);
    void * ptr=k_malloc(100);
}
