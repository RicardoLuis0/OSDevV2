#include "drivers/keyboard/ps2/keyboard.h"
#include "klib.h"
#include "print.h"
#include "arch/x86.h"
#include <stdatomic.h>
#include "util/spinlock.h"
#include "util/lfq.h"


namespace Drivers::Keyboard::PS2 {
    // https://wiki.osdev.org/PS/2_Keyboard
    
    static volatile atomic_bool wait_for_key=false;
    
    Util::LFQ<keycode,20,true> * key_queue;
    
    constexpr const uint8_t KEYCODES_US_QWERTY_SET1_LEN=0xE0;
    constexpr const uint8_t KEYCODES_US_QWERTY_SET1_EXTRA_LEN=0xEE;
    
    //https://wiki.osdev.org/PS/2_Keyboard
    //https://wiki.osdev.org/%228042%22_PS/2_Controller
    
    static inline bool has_scancode(){
        return (inb(0x64)&1);//read status register, check if 
    }
    
    static inline uint8_t get_scancode(){
        return inb(0x60);
    }
    
    static inline uint8_t get_scancode_wait(){
        while(!(inb(0x64)&1))asm("pause");
        return inb(0x60);
    }
    
    static inline keycode get_keycode(){
        uint8_t scancode=get_scancode();
        if(scancode==0xE0){
            scancode=get_scancode_wait();
            if(scancode==0x2A){
                if(get_scancode_wait()==0xE0&&get_scancode_wait()==0x37)return KEY_PRINTSCR_PRESSED;
            }else if(scancode==0xB7){
                if(get_scancode_wait()==0xE0&&get_scancode_wait()==0xAA)return KEY_PRINTSCR_RELEASED;
            }else if(scancode<KEYCODES_US_QWERTY_SET1_EXTRA_LEN){
                return static_cast<keycode>(keycodes_extra1[scancode]);
            }
        }else if(scancode==0xE1){
            if(get_scancode_wait()==0x1D&&get_scancode_wait()==0x45&&get_scancode_wait()==0xE1&&get_scancode_wait()==0x9D&&get_scancode_wait()==0xC5)return KEY_PAUSE;
        }else if(scancode<KEYCODES_US_QWERTY_SET1_LEN){
            return static_cast<keycode>(keycodes[scancode]);
        }
        return KEY_INVALID;//fallback to invalid
    }
    
    static Util::Spinlock get_key_lock;
    
    keycode getKey(){
        Util::SpinlockGuard guard(get_key_lock);//only allow one call to getKey at a time
        if(key_queue->is_empty()){
            wait_for_key=true;
            while(wait_for_key){
                asm("pause");
            }
        }
        return key_queue->pop();
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
            }else switch(keycode){
                case '\'': return '"';
                case  '1': return '!';
                case  '2': return '@';
                case  '3': return '#';
                case  '4': return '$';
                case  '5': return '%';
                case  '6': return '^';
                case  '7': return '&';
                case  '8': return '*';
                case  '9': return '(';
                case  '0': return ')';
                case  '-': return '_';
                case  '=': return '+';
                case  '[': return '{';
                case  ']': return '}';
                case  ',': return '<';
                case  '.': return '>';
                case  ';': return ':';
                case  '/': return '?';
            }
            //TODO handle other shift uses
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
            case KEY_RIGHT_CONTROL_PRESSED:
                return K_GETCH_EXT_CTRL;
            case KEY_LEFT_ALT_PRESSED:
            case KEY_RIGHT_ALT_PRESSED:
                return K_GETCH_EXT_ALT;
            default:
                if(keycode<128){//if is ascii, return
                    return convert_ascii(keycode);
                }
                break;
            }
        }
        return 0;
    }
    
    void update_state(keycode key){
        switch(key){
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
    }
    
    static void kbint(){
        if(has_scancode()){
            keycode last_key=get_keycode();
            update_state(last_key);
            key_queue->push(last_key);
            wait_for_key=false;
        }
    }
    
    void init(){
        print("\n  .PS/2 Keyboard...");
        key_queue=new Util::LFQ<keycode,20,true>();
        uint8_t src=IDT::is_legacy_mode()?0x1:ACPI::MADT::resolve_irq_source(0x1);
        if(IDT::irq_supports_remapping(src)){
            IDT::irq_remap(src,0x21);
        }
        uint8_t irq=IDT::irq_get_mapping(src);
        IDT::set_irq_handler(irq,kbint,IDT::G_32_INT,IDT::RING_0);
        IDT::irq_enable(src);
        Screen::setfgcolor(Screen::LIGHT_GREEN);
        Screen::write_s("OK");
        Screen::setfgcolor(Screen::WHITE);
    }
    
    static uint8_t get_scancode_dump(){
        uint8_t scancode=inb(0x60);
        print("\nKBD:");
        Screen::write_h(scancode);
        return scancode;
    }
    
    static uint8_t get_scancode_dump_wait(){
        while(!(inb(0x64)&1))asm("pause");
        uint8_t scancode=inb(0x60);
        print("\nKBD:");
        Screen::write_h(scancode);
        return scancode;
    }
    
    static inline keycode get_keycode_dump(){
        uint8_t scancode=get_scancode_dump();
        if(scancode==0xE0){
            scancode=get_scancode_dump_wait();
            if(scancode==0x2A){
                if(get_scancode_dump_wait()==0xE0&&get_scancode_dump_wait()==0x37)return KEY_PRINTSCR_PRESSED;
            }else if(scancode==0xB7){
                if(get_scancode_dump_wait()==0xE0&&get_scancode_dump_wait()==0xAA)return KEY_PRINTSCR_RELEASED;
            }else if(scancode<KEYCODES_US_QWERTY_SET1_EXTRA_LEN){
                return static_cast<keycode>(keycodes_extra1[scancode]);
            }
        }else if(scancode==0xE1){
            if(get_scancode_dump_wait()==0x1D&&get_scancode_dump_wait()==0x45&&get_scancode_dump_wait()==0xE1&&get_scancode_dump_wait()==0x9D&&get_scancode_dump_wait()==0xC5)return KEY_PAUSE;
        }else if(scancode<KEYCODES_US_QWERTY_SET1_LEN){
            return static_cast<keycode>(keycodes[scancode]);
        }
        return KEY_INVALID;//fallback to invalid
    }
    
    volatile bool kbdump_continue;
    
    void kbint_dump(){
        if(has_scancode()){
            keycode k=get_keycode_dump();
            update_state(k);
            if((left_ctrl_down||right_ctrl_down)&&k==KEY_C_PRESSED){
                kbdump_continue=false;
            }
            print("\n(");
            print(keycode_name(k));
            print(")");
        }
    }
    
    void cmd_kbdump(){
        kbdump_continue=true;
        uint8_t irq=IDT::irq_get_mapping(0x1);
        IDT::set_irq_handler(irq,kbint_dump,IDT::G_32_INT,IDT::RING_0);
        while(kbdump_continue){
            asm("pause");
        }
        IDT::set_irq_handler(irq,kbint,IDT::G_32_INT,IDT::RING_0);
    }
    
    char simple_getch(){
    start:
        while(!has_scancode());//wait for key
        keycode k=get_keycode();
        if(k<128) return k;
        goto start;
    }
    
}
