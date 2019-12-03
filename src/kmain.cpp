#include "print.h"

void abort(){
    print("\nKernel Aborted, halting execution...");
    asm volatile("jmp hang");
}

void abort(int code){
    print("\nKernel Aborted with code '",code,"', halting execution...");
    asm volatile("jmp hang");
}

extern "C" void kmain(){
    Screen::init();
    print("Hello, World!\nThis is a Test!");
}
