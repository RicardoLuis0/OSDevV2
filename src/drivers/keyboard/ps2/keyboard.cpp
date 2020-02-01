#include "drivers/keyboard/ps2/keyboard.h"
#include "klib.h"
#include "print.h"
#include "idt.h"
#include "stdc/stdatomic.h"
#include "util/spinlock.h"

namespace Drivers {
namespace Keyboard {
namespace PS2 {
// https://wiki.osdev.org/PS/2_Keyboard

static atomic_bool wait_for_key=false;

constexpr const uint8_t KEYCODES_US_QWERTY_SET1_LEN=0xE0;
constexpr const uint8_t KEYCODES_US_QWERTY_SET1_EXTRA_LEN=0xEE;

//https://wiki.osdev.org/PS/2_Keyboard
//https://wiki.osdev.org/%228042%22_PS/2_Controller

static inline bool has_scancode(){
    return (inb(0x64) & 1);//read status register, check if 
}

static inline uint8_t get_scancode(){
    return inb(0x60);
}

static inline keycode get_keycode(){
    uint8_t scancode=get_scancode();
    if(scancode==0xE0){
        scancode=get_scancode();
        if(scancode==0x2A){
            if(get_scancode()==0xE0&&get_scancode()==0x37)return KEY_PRINTSCR_PRESSED;
        }else if(scancode==0xB7){
            if(get_scancode()==0xE0&&get_scancode()==0xAA)return KEY_PRINTSCR_RELEASED;
        }else if(scancode<KEYCODES_US_QWERTY_SET1_EXTRA_LEN){
            return (keycode)keycodes_extra1[scancode];
        }
    }else if(scancode==0xE1){
        if(get_scancode()==0x1D&&get_scancode()==0x45&&get_scancode()==0xE1&&get_scancode()==0x9D&&get_scancode()==0xC5)return KEY_PAUSE;
    }else if(scancode<KEYCODES_US_QWERTY_SET1_LEN){
        return (keycode)keycodes[scancode];
    }
    return KEY_INVALID;//fallback to invalid
}

static Util::Spinlock get_key_lock;

static keycode last_key;

keycode getKey(){
    Util::SpinlockGuard guard(get_key_lock);//only allow one call to getKey at a time
    wait_for_key=true;
    while(wait_for_key){
        asm("pause");
    }
    return last_key;
}

static bool left_shift_down=false;
static bool right_shift_down=false;

static bool left_ctrl_down=false;
static bool right_ctrl_down=false;

static bool left_alt_down=false;
static bool right_alt_down=false;

int convert_ascii(int keycode){
    if(left_shift_down||right_shift_down){
        if(keycode>='a'&&keycode<='z'){//handle uppercase
            return keycode+('A'-'a');
        }//TODO handle other shift uses
    }
    return keycode;
}

extern "C" int k_getch(){//next ascii key
    while(uint16_t keycode=getKey()){//read next key until ascii
        if(keycode<128){//if is in ascii range, return converted keycode
            return convert_ascii(keycode);
        }
    }
    return 0;
}

extern "C" int k_getch_extended(){
    while(uint16_t keycode=getKey()){//read next key until ascii
        switch(keycode){
        case KEY_UP_PRESSED:
            return K_GETCH_EXT_UP;
        case KEY_DOWN_PRESSED:
            return K_GETCH_EXT_DOWN;
        case KEY_LEFT_PRESSED:
            return K_GETCH_EXT_LEFT;
        case KEY_RIGHT_PRESSED:
            return K_GETCH_EXT_RIGHT;
        case KEY_LEFT_CONTROL_PRESSED:
            return K_GETCH_EXT_CTRL;
        case KEY_RIGHT_CONTROL_PRESSED:
            return K_GETCH_EXT_CTRL;
        default:
            if(keycode<128){//if is ascii, return
                return convert_ascii(keycode);
            }
            break;
        }
    }
    return 0;
}

static void kbint(){
    if(has_scancode()){
        last_key=get_keycode();
        switch(last_key){
        case KEY_LEFT_SHIFT_PRESSED:
            left_shift_down=true;
            break;
        case KEY_RIGHT_SHIFT_PRESSED:
            right_shift_down=true;
            break;
        case KEY_LEFT_SHIFT_RELEASED:
            left_shift_down=false;
            break;
        case KEY_RIGHT_SHIFT_RELEASED:
            right_shift_down=false;
            break;
        case KEY_LEFT_CONTROL_PRESSED:
            left_ctrl_down=true;
            break;
        case KEY_RIGHT_CONTROL_PRESSED:
            right_ctrl_down=true;
            break;
        case KEY_LEFT_CONTROL_RELEASED:
            left_ctrl_down=false;
            break;
        case KEY_RIGHT_CONTROL_RELEASED:
            right_ctrl_down=false;
            break;
        case KEY_LEFT_ALT_PRESSED:
            left_alt_down=true;
            break;
        case KEY_RIGHT_ALT_PRESSED:
            right_alt_down=true;
            break;
        case KEY_LEFT_ALT_RELEASED:
            left_alt_down=false;
            break;
        case KEY_RIGHT_ALT_RELEASED:
            right_alt_down=false;
            break;
        default:
            break;
        }
        wait_for_key=false;
    }
}

void init(){
    print("\n -Initializing PS/2 Keyboard...");
    print("\n  .Registering IRQ Handler...");
    IDT::set_irq_handler(0x21,kbint,IDT::G_32_INT,IDT::RING_0);
    print("\n  .Enabling Keyboard Interrupts...");
    IDT::pic_enable(1);
}

static uint8_t get_scancode_dump(){
    uint8_t scancode=get_scancode();
    print("\nKBD:");
    Screen::write_h(scancode);
    return scancode;
}

static inline keycode get_keycode_dump(){
    uint8_t scancode=get_scancode_dump();
    if(scancode==0xE0){
        scancode=get_scancode_dump();
        if(scancode==0x2A){
            if(get_scancode_dump()==0xE0&&get_scancode_dump()==0x37)return KEY_PRINTSCR_PRESSED;
        }else if(scancode==0xB7){
            if(get_scancode_dump()==0xE0&&get_scancode_dump()==0xAA)return KEY_PRINTSCR_RELEASED;
        }else if(scancode<KEYCODES_US_QWERTY_SET1_EXTRA_LEN){
            return (keycode)keycodes_extra1[scancode];
        }
    }else if(scancode==0xE1){
        if(get_scancode_dump()==0x1D&&get_scancode_dump()==0x45&&get_scancode_dump()==0xE1&&get_scancode_dump()==0x9D&&get_scancode_dump()==0xC5)return KEY_PAUSE;
    }else if(scancode<KEYCODES_US_QWERTY_SET1_LEN){
        return (keycode)keycodes[scancode];
    }
    return KEY_INVALID;//fallback to invalid
}

void kbint_dump(){
    if(has_scancode()){
        keycode k=get_keycode_dump();
        print("(");
        print(keycode_name(k));
        print(")");
    }
}

void kbdump(){
    IDT::set_irq_handler(0x21,kbint_dump,IDT::G_32_INT,IDT::RING_0);
    for(;;){
        asm("pause");
    }
}

}}}
