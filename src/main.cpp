#include "klib.h"
#include "print.h"
#include "mem.h"
#include "stdc/assert.h"
#include "drivers/keyboard/ps2/keyboard.h"

#include "kshell/kshell.h"

#include "util/shared_ptr.h"
#include "util/hash_table.h"

extern "C" [[noreturn]] void hang();

extern "C" [[noreturn]] void k_halt(){
    //TODO clean up system before hang
    hang();
}

[[noreturn]] static inline void k_abort_main(){
    Screen::disable_cursor();
    k_halt();
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

extern "C" void k_main(){
    Screen::write_s("\n>Starting Kernel");
    Screen::write_s("\n -Initializing Drivers");
    Drivers::Keyboard::PS2::init();
    //TODO PATA/IDE driver
    //TODO SATA/AHCI driver
    //TODO VGA driver
    Screen::write_s("\n>Loading Kernel Shell");
    kshell();
    k_abort_s("Kernel Shell Returned!");
}
