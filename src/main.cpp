#include "klib.h"
#include "screen.h"
#include "drivers/keyboard/ps2/keyboard.h"
#include "kshell/kshell.h"

void klib_init();

extern "C" void k_main(){
    klib_init();
    Screen::write_s("\n>Starting Kernel");
    Screen::write_s("\n -Initializing Drivers");
    Drivers::Keyboard::PS2::init();
    Screen::write_s("\n>Loading Kernel Shell");
    kshell_init();
    kshell();
    k_abort_s("Kernel Shell Returned!");
}
