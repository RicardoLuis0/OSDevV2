#ifndef KEYBOARD_H_INCLUDED
#define KEYBOARD_H_INCLUDED

#include "drivers/keyboard/ps2/keycodes.h"

namespace Drivers{
namespace Keyboard {
namespace PS2{
    void init();
    [[noreturn]] void cmd_kbdump();
    keycode getKey();
    const char * keycode_name(keycode c);
    
    char simple_getch();//doesn't use interrupts, for pre-driver init use only
    
}}}

#endif // KEYBOARD_H_INCLUDED
