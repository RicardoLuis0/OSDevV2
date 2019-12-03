#include "screen.h"

void abort(){
    Screen::write_s("\nKernel Aborted, halting execution...");
    asm volatile("jmp hang");
}

void abort(int code){
    Screen::write_s("\nKernel Aborted with code '");
    Screen::write_i(code);
    Screen::write_s("', halting execution...");
    asm volatile("jmp hang");
}

extern "C" void kmain(){
    Screen::init();
    Screen::write_s("Hello, World!");
}
