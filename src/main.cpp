#include "klib.h"
#include "print.h"
#include "mem.h"

extern "C" void k_abort(){
    print("\nKernel Aborted, halting execution...");
    asm volatile("jmp hang");
}

extern "C" void k_abort_i(int code){
    print("\nKernel Aborted with code '",code,"', halting execution...");
    asm volatile("jmp hang");
}

extern "C" void k_abort_s(const char * msg){
    print("\nKernel Aborted with message '",msg,"', halting execution...");
    asm volatile("jmp hang");
}

extern "C" void k_main(){
    Screen::init();
    Memory::init();
    print("Hello, World!\nThis is a Test!");
    void * ptr=k_malloc(100);
}
