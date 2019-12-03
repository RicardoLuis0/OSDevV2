#include "screen.h"

void abort(){
    Screen::newline();
    Screen::write("Kernel Aborted, halting execution...");
    asm volatile("jmp hang");
}

void abort(int code){
    Screen::newline();
    Screen::write("Kernel Aborted with code '");
    Screen::write(code);
    Screen::write("', halting execution...");
    asm volatile("jmp hang");
}

extern "C" void kmain(){
    Screen::init();
    Screen::write("Hello, World!");
}
