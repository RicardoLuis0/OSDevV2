#include "drivers/ps2/keyboard.h"
#include "klib.h"
#include "print.h"

namespace Drivers{
namespace PS2{
namespace Keyboard {
    // https://wiki.osdev.org/PS/2_Keyboard
    constexpr const uint16_t keycodes_us_qwerty_set1[] = {
        KEY_INVALID,//0x00 invalid
        KEY_ESC_PRESSED,//0x01 ESCAPE
        KEY_1_PRESSED,//0x02
        KEY_2_PRESSED,//0x03
        KEY_3_PRESSED,//0x04
        KEY_4_PRESSED,//0x05
        KEY_5_PRESSED,//0x06
        KEY_6_PRESSED,//0x07
        KEY_7_PRESSED,//0x08
        KEY_8_PRESSED,//0x09
        KEY_9_PRESSED,//0x0A
        KEY_0_PRESSED,//0x0B
        KEY_MINUS_PRESSED,//0x0C
        KEY_EQUALS_PRESSED,//0x0D
        KEY_BACKSPACE_PRESSED,//0x0E
        KEY_TAB_PRESSED,//0x0F
        KEY_Q_PRESSED,//0x10
        KEY_W_PRESSED,//0x11
        KEY_E_PRESSED,//0x12
        KEY_R_PRESSED,//0x13
        KEY_T_PRESSED,//0x14
        KEY_Y_PRESSED,//0x15
        KEY_U_PRESSED,//0x16
        KEY_I_PRESSED,//0x17
        KEY_O_PRESSED,//0x18
        KEY_P_PRESSED,//0x19
        KEY_LEFT_BRACKET_PRESSED,//0x1A
        KEY_RIGHT_BRACKET_PRESSED,//0x1B
        KEY_ENTER_PRESSED,//0x1C
        KEY_LEFT_CONTROL_PRESSED,//0x1D
        KEY_A_PRESSED,//0x1E
        KEY_S_PRESSED,//0x1F
        KEY_D_PRESSED,//0x20
        KEY_F_PRESSED,//0x21
        KEY_G_PRESSED,//0x22
        KEY_H_PRESSED,//0x23
        KEY_J_PRESSED,//0x24
        KEY_K_PRESSED,//0x25
        KEY_L_PRESSED,//0x26
        KEY_SEMICOLON_PRESSED,//0x27
        KEY_SINGLE_QUOTE_PRESSED,//0x28
        KEY_BACKTICK_PRESSED,//0x29
        KEY_LEFT_SHIFT_PRESSED,//0x2A
        KEY_BACKSLASH_PRESSED,//0x2B
        KEY_Z_PRESSED,//0x2C
        KEY_X_PRESSED,//0x2D
        KEY_C_PRESSED,//0x2E
        KEY_V_PRESSED,//0x2F
        KEY_B_PRESSED,//0x30
        KEY_N_PRESSED,//0x31
        KEY_M_PRESSED,//0x32
        KEY_COMMA_PRESSED,//0x33
        KEY_DOT_PRESSED,//0x34
        KEY_SLASH_PRESSED,//0x35
        KEY_RIGHT_SHIFT_PRESSED,//0x36
        KEY_KP_STAR_PRESSED,//0x37
        KEY_LEFT_ALT_PRESSED,//0x38
        KEY_SPACE_PRESSED,//0x39
        KEY_CAPSLOCK_PRESSED,//0x3A
        KEY_F1_PRESSED,//0x3B
        KEY_F2_PRESSED,//0x3C
        KEY_F3_PRESSED,//0x3D
        KEY_F4_PRESSED,//0x3E
        KEY_F5_PRESSED,//0x3F
        KEY_F6_PRESSED,//0x40
        KEY_F7_PRESSED,//0x41
        KEY_F8_PRESSED,//0x42
        KEY_F9_PRESSED,//0x43
        KEY_F10_PRESSED,//0x44
        KEY_NUM_LOCK_PRESSED,//0x45
        KEY_SCROLL_LOCK_PRESSED,//0x46
        KEY_KP_7_PRESSED,//0x47
        KEY_KP_8_PRESSED,//0x48
        KEY_KP_9_PRESSED,//0x49
        KEY_KP_MINUS_PRESSED,//0x4A
        KEY_KP_4_PRESSED,//0x4B
        KEY_KP_5_PRESSED,//0x4C
        KEY_KP_6_PRESSED,//0x4D
        KEY_KP_PLUS_PRESSED,//0x4E
        KEY_KP_1_PRESSED,//0x4F
        KEY_KP_2_PRESSED,//0x50
        KEY_KP_3_PRESSED,//0x51
        KEY_KP_0_PRESSED,//0x52
        KEY_KP_DOT_PRESSED,//0x53
        KEY_INVALID,//0x54
        KEY_INVALID,//0x55
        KEY_INVALID,//0x56
        KEY_F11_PRESSED,//0x57
        KEY_F12_PRESSED,//0x58
        KEY_INVALID,//0x59
        KEY_INVALID,//0x5A
        KEY_INVALID,//0x5B
        KEY_INVALID,//0x5C
        KEY_INVALID,//0x5D
        KEY_INVALID,//0x5E
        KEY_INVALID,//0x5F
        KEY_INVALID,//0x60
        KEY_INVALID,//0x61
        KEY_INVALID,//0x62
        KEY_INVALID,//0x63
        KEY_INVALID,//0x64
        KEY_INVALID,//0x65
        KEY_INVALID,//0x66
        KEY_INVALID,//0x67
        KEY_INVALID,//0x68
        KEY_INVALID,//0x69
        KEY_INVALID,//0x6A
        KEY_INVALID,//0x6B
        KEY_INVALID,//0x6C
        KEY_INVALID,//0x6D
        KEY_INVALID,//0x6E
        KEY_INVALID,//0x6F
        KEY_INVALID,//0x70
        KEY_INVALID,//0x71
        KEY_INVALID,//0x72
        KEY_INVALID,//0x73
        KEY_INVALID,//0x74
        KEY_INVALID,//0x75
        KEY_INVALID,//0x76
        KEY_INVALID,//0x77
        KEY_INVALID,//0x78
        KEY_INVALID,//0x79
        KEY_INVALID,//0x7A
        KEY_INVALID,//0x7B
        KEY_INVALID,//0x7C
        KEY_INVALID,//0x7D
        KEY_INVALID,//0x7E
        KEY_INVALID,//0x7F
        KEY_INVALID,//0x80
        //RELEASED
        KEY_ESC_RELEASED,//0x81
        KEY_1_RELEASED,//0x82
        KEY_2_RELEASED,//0x83
        KEY_3_RELEASED,//0x84
        KEY_4_RELEASED,//0x85
        KEY_5_RELEASED,//0x86
        KEY_6_RELEASED,//0x87
        KEY_7_RELEASED,//0x88
        KEY_8_RELEASED,//0x89
        KEY_9_RELEASED,//0x8A
        KEY_0_RELEASED,//0x8B
        KEY_MINUS_RELEASED,//0x8C
        KEY_EQUALS_RELEASED,//0x8D
        KEY_BACKSPACE_RELEASED,//0x8E
        KEY_TAB_RELEASED,//0x9F
        KEY_Q_RELEASED,//0x90
        KEY_W_RELEASED,//0x91
        KEY_E_RELEASED,//0x92
        KEY_R_RELEASED,//0x93
        KEY_T_RELEASED,//0x94
        KEY_Y_RELEASED,//0x95
        KEY_U_RELEASED,//0x96
        KEY_I_RELEASED,//0x97
        KEY_O_RELEASED,//0x98
        KEY_P_RELEASED,//0x99
        KEY_LEFT_BRACKET_RELEASED,//0x9A
        KEY_RIGHT_BRACKET_RELEASED,//0x9B
        KEY_ENTER_RELEASED,//0x9C
        KEY_LEFT_CONTROL_RELEASED,//0x9D
        KEY_A_RELEASED,//0x9E
        KEY_S_RELEASED,//0x9F
        KEY_D_RELEASED,//0xA0
        KEY_F_RELEASED,//0xA1
        KEY_G_RELEASED,//0xA2
        KEY_H_RELEASED,//0xA3
        KEY_J_RELEASED,//0xA4
        KEY_K_RELEASED,//0xA5
        KEY_L_RELEASED,//0xA6
        KEY_SEMICOLON_RELEASED,//0xA7
        KEY_SINGLE_QUOTE_RELEASED,//0xA8
        KEY_BACKTICK_RELEASED,//0xA9
        KEY_LEFT_SHIFT_RELEASED,//0xAA
        KEY_BACKSLASH_RELEASED,//0xAB
        KEY_Z_RELEASED,//0xAC
        KEY_X_RELEASED,//0xAD
        KEY_C_RELEASED,//0xAE
        KEY_V_RELEASED,//0xAF
        KEY_B_RELEASED,//0xB0
        KEY_N_RELEASED,//0xB1
        KEY_M_RELEASED,//0xB2
        KEY_COMMA_RELEASED,//0xB3
        KEY_DOT_RELEASED,//0xB4
        KEY_SLASH_RELEASED,//0xB5
        KEY_RIGHT_SHIFT_RELEASED,//0xB6
        KEY_KP_STAR_RELEASED,//0xB7
        KEY_LEFT_ALT_RELEASED,//0xB8
        KEY_SPACE_RELEASED,//0xB9
        KEY_CAPSLOCK_RELEASED,//0xBA
        KEY_F1_RELEASED,//0xBB
        KEY_F2_RELEASED,//0xBC
        KEY_F3_RELEASED,//0xBD
        KEY_F4_RELEASED,//0xBE
        KEY_F5_RELEASED,//0xBF
        KEY_F6_RELEASED,//0xC0
        KEY_F7_RELEASED,//0xC1
        KEY_F8_RELEASED,//0xC2
        KEY_F9_RELEASED,//0xC3
        KEY_F10_RELEASED,//0xC4
        KEY_NUM_LOCK_RELEASED,//0xC5
        KEY_SCROLL_LOCK_RELEASED,//0xC6
        KEY_KP_7_RELEASED,//0xC7
        KEY_KP_8_RELEASED,//0xC8
        KEY_KP_9_RELEASED,//0xC9
        KEY_KP_MINUS_RELEASED,//0xCA
        KEY_KP_4_RELEASED,//0xCB
        KEY_KP_5_RELEASED,//0xCC
        KEY_KP_6_RELEASED,//0xCD
        KEY_KP_PLUS_RELEASED,//0xCE
        KEY_KP_1_RELEASED,//0xCF
        KEY_KP_2_RELEASED,//0xD0
        KEY_KP_3_RELEASED,//0xD1
        KEY_KP_0_RELEASED,//0xD2
        KEY_KP_DOT_RELEASED,//0xD3
        KEY_INVALID,//0xD4
        KEY_INVALID,//0xD5
        KEY_INVALID,//0xD6
        KEY_F11_RELEASED,//0xD7
        KEY_F12_RELEASED,//0xD8
        KEY_INVALID,//0xD9
        KEY_INVALID,//0xDA
        KEY_INVALID,//0xDB
        KEY_INVALID,//0xDC
        KEY_INVALID,//0xDD
        KEY_INVALID,//0xDE
        KEY_INVALID,//0xDF
    };
    constexpr const uint16_t keycodes_us_qwerty_set1_extra[] = {
        KEY_INVALID,//0x00
        KEY_INVALID,//0x01
        KEY_INVALID,//0x02
        KEY_INVALID,//0x03
        KEY_INVALID,//0x04
        KEY_INVALID,//0x05
        KEY_INVALID,//0x06
        KEY_INVALID,//0x07
        KEY_INVALID,//0x08
        KEY_INVALID,//0x09
        KEY_INVALID,//0x0A
        KEY_INVALID,//0x0B
        KEY_INVALID,//0x0C
        KEY_INVALID,//0x0D
        KEY_INVALID,//0x0E
        KEY_INVALID,//0x0F
        KEY_INVALID,//0x10,MULTIMEDIA
        KEY_INVALID,//0x11
        KEY_INVALID,//0x12
        KEY_INVALID,//0x13
        KEY_INVALID,//0x14
        KEY_INVALID,//0x15
        KEY_INVALID,//0x16
        KEY_INVALID,//0x17
        KEY_INVALID,//0x18
        KEY_INVALID,//0x19,MULTIMEDIA
        KEY_INVALID,//0x1A
        KEY_INVALID,//0x1B
        KEY_KP_ENTER_PRESSED,//0x1C
        KEY_RIGHT_CONTROL_PRESSED,//0x1D
        KEY_INVALID,//0x1E
        KEY_INVALID,//0x1F
        KEY_INVALID,//0x20,MULTIMEDIA
        KEY_INVALID,//0x21,MULTIMEDIA
        KEY_INVALID,//0x22,MULTIMEDIA
        KEY_INVALID,//0x23
        KEY_INVALID,//0x24,MULTIMEDIA
        KEY_INVALID,//0x25
        KEY_INVALID,//0x26
        KEY_INVALID,//0x27
        KEY_INVALID,//0x28
        KEY_INVALID,//0x29
        KEY_INVALID,//0x2A
        KEY_INVALID,//0x2B
        KEY_INVALID,//0x2C
        KEY_INVALID,//0x2D
        KEY_INVALID,//0x2E,MULTIMEDIA
        KEY_INVALID,//0x2F
        KEY_INVALID,//0x30,MULTIMEDIA
        KEY_INVALID,//0x31
        KEY_INVALID,//0x32,MULTIMEDIA
        KEY_INVALID,//0x33
        KEY_INVALID,//0x34
        KEY_KP_SLASH_PRESSED,//0x35
        KEY_INVALID,//0x36
        KEY_INVALID,//0x37
        KEY_RIGHT_ALT_PRESSED,//0x38
        KEY_INVALID,//0x39
        KEY_INVALID,//0x3A
        KEY_INVALID,//0x3B
        KEY_INVALID,//0x3C
        KEY_INVALID,//0x3D
        KEY_INVALID,//0x3E
        KEY_INVALID,//0x3F
        KEY_INVALID,//0x40
        KEY_INVALID,//0x41
        KEY_INVALID,//0x42
        KEY_INVALID,//0x43
        KEY_INVALID,//0x44
        KEY_INVALID,//0x45
        KEY_INVALID,//0x46
        KEY_HOME_PRESSED,//0x47
        KEY_UP_PRESSED,//0x48
        KEY_PGUP_PRESSED,//0x49
        KEY_INVALID,//0x4A
        KEY_LEFT_PRESSED,//0x4B
        KEY_INVALID,//0x4C
        KEY_RIGHT_PRESSED,//0x4D
        KEY_INVALID,//0x4E
        KEY_END_PRESSED,//0x4F
        KEY_DOWN_PRESSED,//0x50
        KEY_DOWN_PRESSED,//0x51
        KEY_INSERT_PRESSED,//0x52
        KEY_DELETE_PRESSED,//0x53
        KEY_INVALID,//0x54
        KEY_INVALID,//0x55
        KEY_INVALID,//0x56
        KEY_INVALID,//0x57
        KEY_INVALID,//0x58
        KEY_INVALID,//0x59
        KEY_INVALID,//0x5A
        KEY_LEFT_CMD_PRESSED,//0x5B
        KEY_RIGHT_CMD_PRESSED,//0x5C
        KEY_APPS_PRESSED,//0x5D
        KEY_ACPI_POWER_PRESSED,//0x5E
        KEY_ACPI_SLEEP_PRESSED,//0x5F
        KEY_INVALID,//0x60
        KEY_INVALID,//0x61
        KEY_INVALID,//0x62
        KEY_ACPI_WAKE_PRESSED,//0x63
        KEY_INVALID,//0x64
        KEY_INVALID,//0x65,MULTIMEDIA
        KEY_INVALID,//0x66,MULTIMEDIA
        KEY_INVALID,//0x67,MULTIMEDIA
        KEY_INVALID,//0x68,MULTIMEDIA
        KEY_INVALID,//0x69,MULTIMEDIA
        KEY_INVALID,//0x6A,MULTIMEDIA
        KEY_INVALID,//0x6B,MULTIMEDIA
        KEY_INVALID,//0x6C,MULTIMEDIA
        KEY_INVALID,//0x6D,MULTIMEDIA
        KEY_INVALID,//0x6E
        KEY_INVALID,//0x6F
        KEY_INVALID,//0x70
        KEY_INVALID,//0x71
        KEY_INVALID,//0x72
        KEY_INVALID,//0x73
        KEY_INVALID,//0x74
        KEY_INVALID,//0x75
        KEY_INVALID,//0x76
        KEY_INVALID,//0x77
        KEY_INVALID,//0x78
        KEY_INVALID,//0x79
        KEY_INVALID,//0x7A
        KEY_INVALID,//0x7B
        KEY_INVALID,//0x7C
        KEY_INVALID,//0x7D
        KEY_INVALID,//0x7E
        KEY_INVALID,//0x7F
        KEY_INVALID,//0x80
        KEY_INVALID,//0x81
        KEY_INVALID,//0x82
        KEY_INVALID,//0x83
        KEY_INVALID,//0x84
        KEY_INVALID,//0x85
        KEY_INVALID,//0x86
        KEY_INVALID,//0x87
        KEY_INVALID,//0x88
        KEY_INVALID,//0x89
        KEY_INVALID,//0x8A
        KEY_INVALID,//0x8B
        KEY_INVALID,//0x8C
        KEY_INVALID,//0x8D
        KEY_INVALID,//0x8E
        KEY_INVALID,//0x8F
        //RELEASED
        KEY_INVALID,//0x90,MULTIMEDIA
        KEY_INVALID,//0x91
        KEY_INVALID,//0x92
        KEY_INVALID,//0x93
        KEY_INVALID,//0x94
        KEY_INVALID,//0x95
        KEY_INVALID,//0x96
        KEY_INVALID,//0x97
        KEY_INVALID,//0x98
        KEY_INVALID,//0x99,MULTIMEDIA
        KEY_INVALID,//0x9A
        KEY_INVALID,//0x9B
        KEY_KP_ENTER_RELEASED,//0x9C
        KEY_RIGHT_CONTROL_RELEASED,//0x9D
        KEY_INVALID,//0x9E
        KEY_INVALID,//0x9F
        KEY_INVALID,//0xA0,MULTIMEDIA
        KEY_INVALID,//0xA1,MULTIMEDIA
        KEY_INVALID,//0xA2,MULTIMEDIA
        KEY_INVALID,//0xA3
        KEY_INVALID,//0xA4,MULTIMEDIA
        KEY_INVALID,//0xA5
        KEY_INVALID,//0xA6
        KEY_INVALID,//0xA7
        KEY_INVALID,//0xA8
        KEY_INVALID,//0xA9
        KEY_INVALID,//0xAA
        KEY_INVALID,//0xAB
        KEY_INVALID,//0xAC
        KEY_INVALID,//0xAD
        KEY_INVALID,//0xAE,MULTIMEDIA
        KEY_INVALID,//0xAF
        KEY_INVALID,//0xB0,MULTIMEDIA
        KEY_INVALID,//0xB1
        KEY_INVALID,//0xB2,MULTIMEDIA
        KEY_INVALID,//0xB3
        KEY_INVALID,//0xB4
        KEY_KP_SLASH_RELEASED,//0xB5
        KEY_INVALID,//0xB6
        KEY_INVALID,//0xB7
        KEY_RIGHT_ALT_RELEASED,//0xB8
        KEY_INVALID,//0xB9
        KEY_INVALID,//0xBA
        KEY_INVALID,//0xBB
        KEY_INVALID,//0xBC
        KEY_INVALID,//0xBD
        KEY_INVALID,//0xBE
        KEY_INVALID,//0xBF
        KEY_INVALID,//0xC0
        KEY_INVALID,//0xC1
        KEY_INVALID,//0xC2
        KEY_INVALID,//0xC3
        KEY_INVALID,//0xC4
        KEY_INVALID,//0xC5
        KEY_INVALID,//0xC6
        KEY_HOME_RELEASED,//0xC7
        KEY_UP_RELEASED,//0xC8
        KEY_PGUP_RELEASED,//0xC9
        KEY_INVALID,//0xCA
        KEY_LEFT_RELEASED,//0xCB
        KEY_INVALID,//0xCC
        KEY_RIGHT_RELEASED,//0xCD
        KEY_INVALID,//0xCE
        KEY_END_RELEASED,//0xCF
        KEY_DOWN_RELEASED,//0xD0
        KEY_DOWN_RELEASED,//0xD1
        KEY_INSERT_RELEASED,//0xD2
        KEY_DELETE_RELEASED,//0xD3
        KEY_INVALID,//0xD4
        KEY_INVALID,//0xD5
        KEY_INVALID,//0xD6
        KEY_INVALID,//0xD7
        KEY_INVALID,//0xD8
        KEY_INVALID,//0xD9
        KEY_INVALID,//0xDA
        KEY_LEFT_CMD_RELEASED,//0xDB
        KEY_RIGHT_CMD_RELEASED,//0xDC
        KEY_APPS_RELEASED,//0xDD
        KEY_ACPI_POWER_RELEASED,//0xDE
        KEY_ACPI_SLEEP_RELEASED,//0xDF
        KEY_INVALID,//0xE0
        KEY_INVALID,//0xE1
        KEY_INVALID,//0xE2
        KEY_ACPI_WAKE_RELEASED,//0xE3
        KEY_INVALID,//0xE4
        KEY_INVALID,//0xE5,MULTIMEDIA
        KEY_INVALID,//0xE6,MULTIMEDIA
        KEY_INVALID,//0xE7,MULTIMEDIA
        KEY_INVALID,//0xE8,MULTIMEDIA
        KEY_INVALID,//0xE9,MULTIMEDIA
        KEY_INVALID,//0xEA,MULTIMEDIA
        KEY_INVALID,//0xEB,MULTIMEDIA
        KEY_INVALID,//0xEC,MULTIMEDIA
        KEY_INVALID,//0xED,MULTIMEDIA
        
    };
    constexpr const uint8_t KEYCODES_US_QWERTY_SET1_LEN=0xE0;
    constexpr const uint8_t KEYCODES_US_QWERTY_SET1_EXTRA_LEN=0xEE;
}}}

//https://wiki.osdev.org/PS/2_Keyboard
//https://wiki.osdev.org/%228042%22_PS/2_Controller

using namespace Drivers::PS2::Keyboard;

using Drivers::PS2::Keyboard::keycode;

void Drivers::PS2::Keyboard::init(){//TODO change to interrupts when enabled
    print(" -Initializing PS/2 Keyboard Driver...\n");
}

static inline bool has_scancode(){
    return (inb(0x64) & 1);//read status register, check if 
}

static inline uint8_t get_scancode(){
    while(!has_scancode());
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
            return (keycode)keycodes_us_qwerty_set1_extra[scancode];
        }
    }else if(scancode==0xE1){
        if(get_scancode()==0x1D&&get_scancode()==0x45&&get_scancode()==0xE1&&get_scancode()==0x9D&&get_scancode()==0xC5)return KEY_PAUSE;
    }else if(scancode<KEYCODES_US_QWERTY_SET1_LEN){
        return (keycode)keycodes_us_qwerty_set1[scancode];
    }
    return KEY_INVALID;//fallback to invalid
}

bool Drivers::PS2::Keyboard::hasKey(){
    return has_scancode();
}

keycode Drivers::PS2::Keyboard::getKey(){
    while(!has_scancode());//wait for key
    return get_keycode();//fallback to invalid
}

const char * Drivers::PS2::Keyboard::keycode_name(keycode c){
    switch(c){
    case KEY_INVALID:
        return "KEY_INVALID";
    case KEY_0_PRESSED:
        return "KEY_0_PRESSED";
    case KEY_1_PRESSED:
        return "KEY_1_PRESSED";
    case KEY_2_PRESSED:
        return "KEY_2_PRESSED";
    case KEY_3_PRESSED:
        return "KEY_3_PRESSED";
    case KEY_4_PRESSED:
        return "KEY_4_PRESSED";
    case KEY_5_PRESSED:
        return "KEY_5_PRESSED";
    case KEY_6_PRESSED:
        return "KEY_6_PRESSED";
    case KEY_7_PRESSED:
        return "KEY_7_PRESSED";
    case KEY_8_PRESSED:
        return "KEY_8_PRESSED";
    case KEY_9_PRESSED:
        return "KEY_9_PRESSED";
    case KEY_A_PRESSED:
        return "KEY_A_PRESSED";
    case KEY_B_PRESSED:
        return "KEY_B_PRESSED";
    case KEY_C_PRESSED:
        return "KEY_C_PRESSED";
    case KEY_D_PRESSED:
        return "KEY_D_PRESSED";
    case KEY_E_PRESSED:
        return "KEY_E_PRESSED";
    case KEY_F_PRESSED:
        return "KEY_F_PRESSED";
    case KEY_G_PRESSED:
        return "KEY_G_PRESSED";
    case KEY_H_PRESSED:
        return "KEY_H_PRESSED";
    case KEY_I_PRESSED:
        return "KEY_I_PRESSED";
    case KEY_J_PRESSED:
        return "KEY_J_PRESSED";
    case KEY_K_PRESSED:
        return "KEY_K_PRESSED";
    case KEY_L_PRESSED:
        return "KEY_L_PRESSED";
    case KEY_M_PRESSED:
        return "KEY_M_PRESSED";
    case KEY_N_PRESSED:
        return "KEY_N_PRESSED";
    case KEY_O_PRESSED:
        return "KEY_O_PRESSED";
    case KEY_P_PRESSED:
        return "KEY_P_PRESSED";
    case KEY_Q_PRESSED:
        return "KEY_Q_PRESSED";
    case KEY_R_PRESSED:
        return "KEY_R_PRESSED";
    case KEY_S_PRESSED:
        return "KEY_S_PRESSED";
    case KEY_T_PRESSED:
        return "KEY_T_PRESSED";
    case KEY_U_PRESSED:
        return "KEY_U_PRESSED";
    case KEY_V_PRESSED:
        return "KEY_V_PRESSED";
    case KEY_W_PRESSED:
        return "KEY_W_PRESSED";
    case KEY_X_PRESSED:
        return "KEY_X_PRESSED";
    case KEY_Y_PRESSED:
        return "KEY_Y_PRESSED";
    case KEY_Z_PRESSED:
        return "KEY_Z_PRESSED";
    case KEY_LEFT_BRACKET_PRESSED:
        return "KEY_LEFT_BRACKET_PRESSED";
    case KEY_RIGHT_BRACKET_PRESSED:
        return "KEY_RIGHT_BRACKET_PRESSED";
    case KEY_MINUS_PRESSED:
        return "KEY_MINUS_PRESSED";
    case KEY_EQUALS_PRESSED:
        return "KEY_EQUALS_PRESSED";
    case KEY_ESC_PRESSED:
        return "KEY_ESC_PRESSED";
    case KEY_BACKSPACE_PRESSED:
        return "KEY_BACKSPACE_PRESSED";
    case KEY_TAB_PRESSED:
        return "KEY_TAB_PRESSED";
    case KEY_ENTER_PRESSED:
        return "KEY_ENTER_PRESSED";
    case KEY_SEMICOLON_PRESSED:
        return "KEY_SEMICOLON_PRESSED";
    case KEY_SINGLE_QUOTE_PRESSED:
        return "KEY_SINGLE_QUOTE_PRESSED";
    case KEY_BACKTICK_PRESSED:
        return "KEY_BACKTICK_PRESSED";
    case KEY_BACKSLASH_PRESSED:
        return "KEY_BACKSLASH_PRESSED";
    case KEY_SPACE_PRESSED:
        return "KEY_SPACE_PRESSED";
    case KEY_COMMA_PRESSED:
        return "KEY_COMMA_PRESSED";
    case KEY_DOT_PRESSED:
        return "KEY_DOT_PRESSED";
    case KEY_SLASH_PRESSED:
        return "KEY_SLASH_PRESSED";
    case SCANCODE_SPECIAL:
        return "SCANCODE_SPECIAL";
    case KEY_LEFT_SHIFT_PRESSED:
        return "KEY_LEFT_SHIFT_PRESSED";
    case KEY_RIGHT_SHIFT_PRESSED:
        return "KEY_RIGHT_SHIFT_PRESSED";
    case KEY_LEFT_CONTROL_PRESSED:
        return "KEY_LEFT_CONTROL_PRESSED";
    case KEY_LEFT_ALT_PRESSED:
        return "KEY_LEFT_ALT_PRESSED";
    case KEY_CAPSLOCK_PRESSED:
        return "KEY_CAPSLOCK_PRESSED";
    case KEY_NUM_LOCK_PRESSED:
        return "KEY_NUM_LOCK_PRESSED";
    case KEY_SCROLL_LOCK_PRESSED:
        return "KEY_SCROLL_LOCK_PRESSED";
    case KEY_F1_PRESSED:
        return "KEY_F1_PRESSED";
    case KEY_F2_PRESSED:
        return "KEY_F2_PRESSED";
    case KEY_F3_PRESSED:
        return "KEY_F3_PRESSED";
    case KEY_F4_PRESSED:
        return "KEY_F4_PRESSED";
    case KEY_F5_PRESSED:
        return "KEY_F5_PRESSED";
    case KEY_F6_PRESSED:
        return "KEY_F6_PRESSED";
    case KEY_F7_PRESSED:
        return "KEY_F7_PRESSED";
    case KEY_F8_PRESSED:
        return "KEY_F8_PRESSED";
    case KEY_F9_PRESSED:
        return "KEY_F9_PRESSED";
    case KEY_F10_PRESSED:
        return "KEY_F10_PRESSED";
    case KEY_F11_PRESSED:
        return "KEY_F11_PRESSED";
    case KEY_F12_PRESSED:
        return "KEY_F12_PRESSED";
    case KEY_KP_STAR_PRESSED:
        return "KEY_KP_STAR_PRESSED";
    case KEY_KP_MINUS_PRESSED:
        return "KEY_KP_MINUS_PRESSED";
    case KEY_KP_PLUS_PRESSED:
        return "KEY_KP_PLUS_PRESSED";
    case KEY_KP_DOT_PRESSED:
        return "KEY_KP_DOT_PRESSED";
    case KEY_KP_0_PRESSED:
        return "KEY_KP_0_PRESSED";
    case KEY_KP_1_PRESSED:
        return "KEY_KP_1_PRESSED";
    case KEY_KP_2_PRESSED:
        return "KEY_KP_2_PRESSED";
    case KEY_KP_3_PRESSED:
        return "KEY_KP_3_PRESSED";
    case KEY_KP_4_PRESSED:
        return "KEY_KP_4_PRESSED";
    case KEY_KP_5_PRESSED:
        return "KEY_KP_5_PRESSED";
    case KEY_KP_6_PRESSED:
        return "KEY_KP_6_PRESSED";
    case KEY_KP_7_PRESSED:
        return "KEY_KP_7_PRESSED";
    case KEY_KP_8_PRESSED:
        return "KEY_KP_8_PRESSED";
    case KEY_KP_9_PRESSED:
        return "KEY_KP_9_PRESSED";
    case KEY_0_RELEASED:
        return "KEY_0_RELEASED";
    case KEY_1_RELEASED:
        return "KEY_1_RELEASED";
    case KEY_2_RELEASED:
        return "KEY_2_RELEASED";
    case KEY_3_RELEASED:
        return "KEY_3_RELEASED";
    case KEY_4_RELEASED:
        return "KEY_4_RELEASED";
    case KEY_5_RELEASED:
        return "KEY_5_RELEASED";
    case KEY_6_RELEASED:
        return "KEY_6_RELEASED";
    case KEY_7_RELEASED:
        return "KEY_7_RELEASED";
    case KEY_8_RELEASED:
        return "KEY_8_RELEASED";
    case KEY_9_RELEASED:
        return "KEY_9_RELEASED";
    case KEY_A_RELEASED:
        return "KEY_A_RELEASED";
    case KEY_B_RELEASED:
        return "KEY_B_RELEASED";
    case KEY_C_RELEASED:
        return "KEY_C_RELEASED";
    case KEY_D_RELEASED:
        return "KEY_D_RELEASED";
    case KEY_E_RELEASED:
        return "KEY_E_RELEASED";
    case KEY_F_RELEASED:
        return "KEY_F_RELEASED";
    case KEY_G_RELEASED:
        return "KEY_G_RELEASED";
    case KEY_H_RELEASED:
        return "KEY_H_RELEASED";
    case KEY_I_RELEASED:
        return "KEY_I_RELEASED";
    case KEY_J_RELEASED:
        return "KEY_J_RELEASED";
    case KEY_K_RELEASED:
        return "KEY_K_RELEASED";
    case KEY_L_RELEASED:
        return "KEY_L_RELEASED";
    case KEY_M_RELEASED:
        return "KEY_M_RELEASED";
    case KEY_N_RELEASED:
        return "KEY_N_RELEASED";
    case KEY_O_RELEASED:
        return "KEY_O_RELEASED";
    case KEY_P_RELEASED:
        return "KEY_P_RELEASED";
    case KEY_Q_RELEASED:
        return "KEY_Q_RELEASED";
    case KEY_R_RELEASED:
        return "KEY_R_RELEASED";
    case KEY_S_RELEASED:
        return "KEY_S_RELEASED";
    case KEY_T_RELEASED:
        return "KEY_T_RELEASED";
    case KEY_U_RELEASED:
        return "KEY_U_RELEASED";
    case KEY_V_RELEASED:
        return "KEY_V_RELEASED";
    case KEY_W_RELEASED:
        return "KEY_W_RELEASED";
    case KEY_X_RELEASED:
        return "KEY_X_RELEASED";
    case KEY_Y_RELEASED:
        return "KEY_Y_RELEASED";
    case KEY_Z_RELEASED:
        return "KEY_Z_RELEASED";
    case KEY_LEFT_BRACKET_RELEASED:
        return "KEY_LEFT_BRACKET_RELEASED";
    case KEY_RIGHT_BRACKET_RELEASED:
        return "KEY_RIGHT_BRACKET_RELEASED";
    case KEY_MINUS_RELEASED:
        return "KEY_MINUS_RELEASED";
    case KEY_EQUALS_RELEASED:
        return "KEY_EQUALS_RELEASED";
    case KEY_ESC_RELEASED:
        return "KEY_ESC_RELEASED";
    case KEY_BACKSPACE_RELEASED:
        return "KEY_BACKSPACE_RELEASED";
    case KEY_TAB_RELEASED:
        return "KEY_TAB_RELEASED";
    case KEY_ENTER_RELEASED:
        return "KEY_ENTER_RELEASED";
    case KEY_SEMICOLON_RELEASED:
        return "KEY_SEMICOLON_RELEASED";
    case KEY_SINGLE_QUOTE_RELEASED:
        return "KEY_SINGLE_QUOTE_RELEASED";
    case KEY_BACKTICK_RELEASED:
        return "KEY_BACKTICK_RELEASED";
    case KEY_BACKSLASH_RELEASED:
        return "KEY_BACKSLASH_RELEASED";
    case KEY_SPACE_RELEASED:
        return "KEY_SPACE_RELEASED";
    case KEY_COMMA_RELEASED:
        return "KEY_COMMA_RELEASED";
    case KEY_DOT_RELEASED:
        return "KEY_DOT_RELEASED";
    case KEY_SLASH_RELEASED:
        return "KEY_SLASH_RELEASED";
    case KEY_LEFT_SHIFT_RELEASED:
        return "KEY_LEFT_SHIFT_RELEASED";
    case KEY_RIGHT_SHIFT_RELEASED:
        return "KEY_RIGHT_SHIFT_RELEASED";
    case KEY_LEFT_CONTROL_RELEASED:
        return "KEY_LEFT_CONTROL_RELEASED";
    case KEY_LEFT_ALT_RELEASED:
        return "KEY_LEFT_ALT_RELEASED";
    case KEY_CAPSLOCK_RELEASED:
        return "KEY_CAPSLOCK_RELEASED";
    case KEY_NUM_LOCK_RELEASED:
        return "KEY_NUM_LOCK_RELEASED";
    case KEY_SCROLL_LOCK_RELEASED:
        return "KEY_SCROLL_LOCK_RELEASED";
    case KEY_F1_RELEASED:
        return "KEY_F1_RELEASED";
    case KEY_F2_RELEASED:
        return "KEY_F2_RELEASED";
    case KEY_F3_RELEASED:
        return "KEY_F3_RELEASED";
    case KEY_F4_RELEASED:
        return "KEY_F4_RELEASED";
    case KEY_F5_RELEASED:
        return "KEY_F5_RELEASED";
    case KEY_F6_RELEASED:
        return "KEY_F6_RELEASED";
    case KEY_F7_RELEASED:
        return "KEY_F7_RELEASED";
    case KEY_F8_RELEASED:
        return "KEY_F8_RELEASED";
    case KEY_F9_RELEASED:
        return "KEY_F9_RELEASED";
    case KEY_F10_RELEASED:
        return "KEY_F10_RELEASED";
    case KEY_F11_RELEASED:
        return "KEY_F11_RELEASED";
    case KEY_F12_RELEASED:
        return "KEY_F12_RELEASED";
    case KEY_KP_STAR_RELEASED:
        return "KEY_KP_STAR_RELEASED";
    case KEY_KP_MINUS_RELEASED:
        return "KEY_KP_MINUS_RELEASED";
    case KEY_KP_PLUS_RELEASED:
        return "KEY_KP_PLUS_RELEASED";
    case KEY_KP_DOT_RELEASED:
        return "KEY_KP_DOT_RELEASED";
    case KEY_KP_0_RELEASED:
        return "KEY_KP_0_RELEASED";
    case KEY_KP_1_RELEASED:
        return "KEY_KP_1_RELEASED";
    case KEY_KP_2_RELEASED:
        return "KEY_KP_2_RELEASED";
    case KEY_KP_3_RELEASED:
        return "KEY_KP_3_RELEASED";
    case KEY_KP_4_RELEASED:
        return "KEY_KP_4_RELEASED";
    case KEY_KP_5_RELEASED:
        return "KEY_KP_5_RELEASED";
    case KEY_KP_6_RELEASED:
        return "KEY_KP_6_RELEASED";
    case KEY_KP_7_RELEASED:
        return "KEY_KP_7_RELEASED";
    case KEY_KP_8_RELEASED:
        return "KEY_KP_8_RELEASED";
    case KEY_KP_9_RELEASED:
        return "KEY_KP_9_RELEASED";
    case KEY_KP_ENTER_PRESSED:
        return "KEY_KP_ENTER_PRESSED";
    case KEY_RIGHT_CONTROL_PRESSED:
        return "KEY_RIGHT_CONTROL_PRESSED";
    case KEY_KP_SLASH_PRESSED:
        return "KEY_KP_SLASH_PRESSED";
    case KEY_RIGHT_ALT_PRESSED:
        return "KEY_RIGHT_ALT_PRESSED";
    case KEY_HOME_PRESSED:
        return "KEY_HOME_PRESSED";
    case KEY_UP_PRESSED:
        return "KEY_UP_PRESSED";
    case KEY_PGUP_PRESSED:
        return "KEY_PGUP_PRESSED";
    case KEY_LEFT_PRESSED:
        return "KEY_LEFT_PRESSED";
    case KEY_RIGHT_PRESSED:
        return "KEY_RIGHT_PRESSED";
    case KEY_END_PRESSED:
        return "KEY_END_PRESSED";
    case KEY_DOWN_PRESSED:
        return "KEY_DOWN_PRESSED";
    case KEY_PGDOWN_PRESSED:
        return "KEY_PGDOWN_PRESSED";
    case KEY_INSERT_PRESSED:
        return "KEY_INSERT_PRESSED";
    case KEY_DELETE_PRESSED:
        return "KEY_DELETE_PRESSED";
    case KEY_LEFT_CMD_PRESSED:
        return "KEY_LEFT_CMD_PRESSED";
    case KEY_RIGHT_CMD_PRESSED:
        return "KEY_RIGHT_CMD_PRESSED";
    case KEY_APPS_PRESSED:
        return "KEY_APPS_PRESSED";
    case KEY_ACPI_POWER_PRESSED:
        return "KEY_ACPI_POWER_PRESSED";
    case KEY_ACPI_SLEEP_PRESSED:
        return "KEY_ACPI_SLEEP_PRESSED";
    case KEY_ACPI_WAKE_PRESSED:
        return "KEY_ACPI_WAKE_PRESSED";
    case KEY_KP_ENTER_RELEASED:
        return "KEY_KP_ENTER_RELEASED";
    case KEY_RIGHT_CONTROL_RELEASED:
        return "KEY_RIGHT_CONTROL_RELEASED";
    case KEY_KP_SLASH_RELEASED:
        return "KEY_KP_SLASH_RELEASED";
    case KEY_RIGHT_ALT_RELEASED:
        return "KEY_RIGHT_ALT_RELEASED";
    case KEY_HOME_RELEASED:
        return "KEY_HOME_RELEASED";
    case KEY_UP_RELEASED:
        return "KEY_UP_RELEASED";
    case KEY_PGUP_RELEASED:
        return "KEY_PGUP_RELEASED";
    case KEY_LEFT_RELEASED:
        return "KEY_LEFT_RELEASED";
    case KEY_RIGHT_RELEASED:
        return "KEY_RIGHT_RELEASED";
    case KEY_END_RELEASED:
        return "KEY_END_RELEASED";
    case KEY_DOWN_RELEASED:
        return "KEY_DOWN_RELEASED";
    case KEY_PGDOWN_RELEASED:
        return "KEY_PGDOWN_RELEASED";
    case KEY_INSERT_RELEASED:
        return "KEY_INSERT_RELEASED";
    case KEY_DELETE_RELEASED:
        return "KEY_DELETE_RELEASED";
    case KEY_LEFT_CMD_RELEASED:
        return "KEY_LEFT_CMD_RELEASED";
    case KEY_RIGHT_CMD_RELEASED:
        return "KEY_RIGHT_CMD_RELEASED";
    case KEY_APPS_RELEASED:
        return "KEY_APPS_RELEASED";
    case KEY_ACPI_POWER_RELEASED:
        return "KEY_ACPI_POWER_RELEASED";
    case KEY_ACPI_SLEEP_RELEASED:
        return "KEY_ACPI_SLEEP_RELEASED";
    case KEY_ACPI_WAKE_RELEASED:
        return "KEY_ACPI_WAKE_RELEASED";
    case KEY_PAUSE:
        return "KEY_PAUSE";
    case KEY_PRINTSCR_PRESSED:
        return "KEY_PRINTSCR_PRESSED";
    case KEY_PRINTSCR_RELEASED:
        return "KEY_PRINTSCR_RELEASED";
    default:
        return "unknown";
    }
}

extern "C" char k_getch(){//next ascii key
    uint16_t keycode;
    static bool left_shift=false;
    static bool right_shift=false;
    while((keycode=get_keycode())){
        if(keycode==KEY_LEFT_SHIFT_PRESSED){
            left_shift=true;
        }else if(keycode==KEY_RIGHT_SHIFT_PRESSED){
            right_shift=true;
        }else if(keycode==KEY_LEFT_SHIFT_RELEASED){
            left_shift=false;
        }else if(keycode==KEY_RIGHT_SHIFT_RELEASED){
            right_shift=false;
        }else if(keycode<=255&&keycode!=KEY_INVALID){//if is ascii
            if(keycode>='a'&&keycode<='z'&&(left_shift||right_shift)){//if is letter, uppercase
                return keycode+('A'-'a');
            }else{
                return keycode;
            }
        }
    }
    return 0;
}
