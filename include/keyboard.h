#ifndef KEYBOARD_H_INCLUDED
#define KEYBOARD_H_INCLUDED

#include "default/stdint.h"

namespace Keyboard {
    enum keycode{
        KEY_INVALID=0,
        KEY_0_DOWN='0',
        KEY_1_DOWN,
        KEY_2_DOWN,
        KEY_3_DOWN,
        KEY_4_DOWN,
        KEY_5_DOWN,
        KEY_6_DOWN,
        KEY_7_DOWN,
        KEY_8_DOWN,
        KEY_9_DOWN,
        KEY_A_DOWN='a',
        KEY_B_DOWN,
        KEY_C_DOWN,
        KEY_D_DOWN,
        KEY_E_DOWN,
        KEY_F_DOWN,
        KEY_G_DOWN,
        KEY_H_DOWN,
        KEY_I_DOWN,
        KEY_J_DOWN,
        KEY_K_DOWN,
        KEY_L_DOWN,
        KEY_M_DOWN,
        KEY_N_DOWN,
        KEY_O_DOWN,
        KEY_P_DOWN,
        KEY_Q_DOWN,
        KEY_R_DOWN,
        KEY_S_DOWN,
        KEY_T_DOWN,
        KEY_U_DOWN,
        KEY_V_DOWN,
        KEY_W_DOWN,
        KEY_X_DOWN,
        KEY_Y_DOWN,
        KEY_Z_DOWN,
        KEY_LEFT_BRACKET_DOWN='[',
        KEY_RIGHT_BRACKET_DOWN=']',
        KEY_MINUS_DOWN='-',
        KEY_EQUALS_DOWN='=',
        KEY_ESC_DOWN=27,
        KEY_BACKSPACE_DOWN='\b',
        KEY_TAB_DOWN='\t',
        KEY_ENTER_DOWN='\n',
        KEY_SEMICOLON_DOWN=';',
        KEY_SINGLE_QUOTE_DOWN='\'',
        KEY_BACKTICK_DOWN='`',
        KEY_BACKSLASH_DOWN='\\',
        KEY_SPACE_DOWN=' ',
        KEY_COMMA_DOWN=',',
        KEY_DOT_DOWN='.',
        KEY_SLASH_DOWN='/',
        SCANCODE_SPECIAL=256,//after this only non-ascii
        KEY_LEFT_SHIFT_DOWN,
        KEY_RIGHT_SHIFT_DOWN,
        KEY_LEFT_CONTRLOL_DOWN,
        KEY_LEFT_ALT_DOWN,
        KEY_CAPSLOCK_DOWN,
        KEY_NUMLOCK_DOWN,
        KEY_SCROLL_LOCK_DOWN,
        KEY_F1_DOWN,
        KEY_F2_DOWN,
        KEY_F3_DOWN,
        KEY_F4_DOWN,
        KEY_F5_DOWN,
        KEY_F6_DOWN,
        KEY_F7_DOWN,
        KEY_F8_DOWN,
        KEY_F9_DOWN,
        KEY_F10_DOWN,
        KEY_F11_DOWN,
        KEY_F12_DOWN,
        KEY_KP_STAR_DOWN,
        KEY_KP_MINUS_DOWN,
        KEY_KP_PLUS_DOWN,
        KEY_KP_DOT_DOWN,
        KEY_KP_0_DOWN,
        KEY_KP_1_DOWN,
        KEY_KP_2_DOWN,
        KEY_KP_3_DOWN,
        KEY_KP_4_DOWN,
        KEY_KP_5_DOWN,
        KEY_KP_6_DOWN,
        KEY_KP_7_DOWN,
        KEY_KP_8_DOWN,
        KEY_KP_9_DOWN,
        KEY_0_UP,
        KEY_1_UP,
        KEY_2_UP,
        KEY_3_UP,
        KEY_4_UP,
        KEY_5_UP,
        KEY_6_UP,
        KEY_7_UP,
        KEY_8_UP,
        KEY_9_UP,
        KEY_A_UP,
        KEY_B_UP,
        KEY_C_UP,
        KEY_D_UP,
        KEY_E_UP,
        KEY_F_UP,
        KEY_G_UP,
        KEY_H_UP,
        KEY_I_UP,
        KEY_J_UP,
        KEY_K_UP,
        KEY_L_UP,
        KEY_M_UP,
        KEY_N_UP,
        KEY_O_UP,
        KEY_P_UP,
        KEY_Q_UP,
        KEY_R_UP,
        KEY_S_UP,
        KEY_T_UP,
        KEY_U_UP,
        KEY_V_UP,
        KEY_W_UP,
        KEY_X_UP,
        KEY_Y_UP,
        KEY_Z_UP,
        KEY_LEFT_BRACKET_UP,
        KEY_RIGHT_BRACKET_UP,
        KEY_MINUS_UP,
        KEY_EQUALS_UP,
        KEY_ESC_UP,
        KEY_BACKSPACE_UP,
        KEY_TAB_UP,
        KEY_ENTER_UP,
        KEY_SEMICOLON_UP,
        KEY_SINGLE_QUOTE_UP,
        KEY_BACKTICK_UP,
        KEY_BACKSLASH_UP,
        KEY_SPACE_UP,
        KEY_COMMA_UP,
        KEY_DOT_UP,
        KEY_SLASH_UP,
        KEY_LEFT_SHIFT_UP,
        KEY_RIGHT_SHIFT_UP,
        KEY_LEFT_CONTRLOL_UP,
        KEY_LEFT_ALT_UP,
        KEY_CAPSLOCK_UP,
        KEY_NUMLOCK_UP,
        KEY_SCROLL_LOCK_UP,
        KEY_F1_UP,
        KEY_F2_UP,
        KEY_F3_UP,
        KEY_F4_UP,
        KEY_F5_UP,
        KEY_F6_UP,
        KEY_F7_UP,
        KEY_F8_UP,
        KEY_F9_UP,
        KEY_F10_UP,
        KEY_F11_UP,
        KEY_F12_UP,
        KEY_KP_STAR_UP,
        KEY_KP_MINUS_UP,
        KEY_KP_PLUS_UP,
        KEY_KP_DOT_UP,
        KEY_KP_0_UP,
        KEY_KP_1_UP,
        KEY_KP_2_UP,
        KEY_KP_3_UP,
        KEY_KP_4_UP,
        KEY_KP_5_UP,
        KEY_KP_6_UP,
        KEY_KP_7_UP,
        KEY_KP_8_UP,
        KEY_KP_9_UP,
        KEY_EXTRA,
    };
    // https://wiki.osdev.org/PS/2_Keyboard
    constexpr const uint16_t scancodes_us_qwerty_set1[] = {
        KEY_INVALID,//0x00 invalid
        KEY_ESC_DOWN,//0x01 ESCAPE
        KEY_1_DOWN,//0x02
        KEY_2_DOWN,//0x03
        KEY_3_DOWN,//0x04
        KEY_4_DOWN,//0x05
        KEY_5_DOWN,//0x06
        KEY_6_DOWN,//0x07
        KEY_7_DOWN,//0x08
        KEY_8_DOWN,//0x09
        KEY_9_DOWN,//0x0A
        KEY_0_DOWN,//0x0B
        KEY_MINUS_DOWN,//0x0C
        KEY_EQUALS_DOWN,//0x0D
        KEY_BACKSPACE_DOWN,//0x0E
        KEY_TAB_DOWN,//0x0F
        KEY_Q_DOWN,//0x10
        KEY_W_DOWN,//0x11
        KEY_E_DOWN,//0x12
        KEY_R_DOWN,//0x13
        KEY_T_DOWN,//0x14
        KEY_Y_DOWN,//0x15
        KEY_U_DOWN,//0x16
        KEY_I_DOWN,//0x17
        KEY_O_DOWN,//0x18
        KEY_P_DOWN,//0x19
        KEY_LEFT_BRACKET_DOWN,//0x1A
        KEY_RIGHT_BRACKET_DOWN,//0x1B
        KEY_ENTER_DOWN,//0x1C
        KEY_LEFT_CONTRLOL_DOWN,//0x1D
        KEY_A_DOWN,//0x1E
        KEY_S_DOWN,//0x1F
        KEY_D_DOWN,//0x20
        KEY_F_DOWN,//0x21
        KEY_G_DOWN,//0x22
        KEY_H_DOWN,//0x23
        KEY_J_DOWN,//0x24
        KEY_K_DOWN,//0x25
        KEY_L_DOWN,//0x26
        KEY_SEMICOLON_DOWN,//0x27
        KEY_SINGLE_QUOTE_DOWN,//0x28
        KEY_BACKTICK_DOWN,//0x29
        KEY_LEFT_SHIFT_DOWN,//0x2A
        KEY_BACKSLASH_DOWN,//0x2B
        KEY_Z_DOWN,//0x2C
        KEY_X_DOWN,//0x2D
        KEY_C_DOWN,//0x2E
        KEY_V_DOWN,//0x2F
        KEY_B_DOWN,//0x30
        KEY_N_DOWN,//0x31
        KEY_M_DOWN,//0x32
        KEY_COMMA_DOWN,//0x33
        KEY_DOT_DOWN,//0x34
        KEY_SLASH_DOWN,//0x35
        KEY_RIGHT_SHIFT_DOWN,//0x36
        KEY_KP_STAR_DOWN,//0x37
        KEY_LEFT_ALT_DOWN,//0x38
        KEY_SPACE_DOWN,//0x39
        KEY_CAPSLOCK_DOWN,//0x3A
        KEY_F1_DOWN,//0x3B
        KEY_F2_DOWN,//0x3C
        KEY_F3_DOWN,//0x3D
        KEY_F4_DOWN,//0x3E
        KEY_F5_DOWN,//0x3F
        KEY_F6_DOWN,//0x40
        KEY_F7_DOWN,//0x41
        KEY_F8_DOWN,//0x42
        KEY_F9_DOWN,//0x43
        KEY_F10_DOWN,//0x44
        KEY_NUMLOCK_DOWN,//0x45
        KEY_SCROLL_LOCK_DOWN,//0x46
        KEY_KP_7_DOWN,//0x47
        KEY_KP_8_DOWN,//0x48
        KEY_KP_9_DOWN,//0x49
        KEY_KP_MINUS_DOWN,//0x4A
        KEY_KP_4_DOWN,//0x4B
        KEY_KP_5_DOWN,//0x4C
        KEY_KP_6_DOWN,//0x4D
        KEY_KP_PLUS_DOWN,//0x4E
        KEY_KP_1_DOWN,//0x4F
        KEY_KP_2_DOWN,//0x50
        KEY_KP_3_DOWN,//0x51
        KEY_KP_0_DOWN,//0x52
        KEY_KP_DOT_DOWN,//0x53
        KEY_INVALID,//0x54
        KEY_INVALID,//0x55
        KEY_INVALID,//0x56
        KEY_F11_DOWN,//0x57
        KEY_F12_DOWN,//0x58
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
        KEY_INVALID,//0x80
        KEY_ESC_UP,//0x81
        KEY_1_UP,//0x82
        KEY_2_UP,//0x83
        KEY_3_UP,//0x84
        KEY_4_UP,//0x85
        KEY_5_UP,//0x86
        KEY_6_UP,//0x87
        KEY_7_UP,//0x88
        KEY_8_UP,//0x89
        KEY_9_UP,//0x8A
        KEY_0_UP,//0x8B
        KEY_MINUS_UP,//0x8C
        KEY_EQUALS_UP,//0x8D
        KEY_BACKSPACE_UP,//0x8E
        KEY_TAB_UP,//0x9F
        KEY_Q_UP,//0x90
        KEY_W_UP,//0x91
        KEY_E_UP,//0x92
        KEY_R_UP,//0x93
        KEY_T_UP,//0x94
        KEY_Y_UP,//0x95
        KEY_U_UP,//0x96
        KEY_I_UP,//0x97
        KEY_O_UP,//0x98
        KEY_P_UP,//0x99
        KEY_LEFT_BRACKET_UP,//0x9A
        KEY_RIGHT_BRACKET_UP,//0x9B
        KEY_ENTER_UP,//0x9C
        KEY_LEFT_CONTRLOL_UP,//0x9D
        KEY_A_UP,//0x9E
        KEY_S_UP,//0x9F
        KEY_D_UP,//0xA0
        KEY_F_UP,//0xA1
        KEY_G_UP,//0xA2
        KEY_H_UP,//0xA3
        KEY_J_UP,//0xA4
        KEY_K_UP,//0xA5
        KEY_L_UP,//0xA6
        KEY_SEMICOLON_UP,//0xA7
        KEY_SINGLE_QUOTE_UP,//0xA8
        KEY_BACKTICK_UP,//0xA9
        KEY_LEFT_SHIFT_UP,//0xAA
        KEY_BACKSLASH_UP,//0xAB
        KEY_Z_UP,//0xAC
        KEY_X_UP,//0xAD
        KEY_C_UP,//0xAE
        KEY_V_UP,//0xAF
        KEY_B_UP,//0xB0
        KEY_N_UP,//0xB1
        KEY_M_UP,//0xB2
        KEY_COMMA_UP,//0xB3
        KEY_DOT_UP,//0xB4
        KEY_SLASH_UP,//0xB5
        KEY_RIGHT_SHIFT_UP,//0xB6
        KEY_KP_STAR_UP,//0xB7
        KEY_LEFT_ALT_UP,//0xB8
        KEY_SPACE_UP,//0xB9
        KEY_CAPSLOCK_UP,//0xBA
        KEY_F1_UP,//0xBB
        KEY_F2_UP,//0xBC
        KEY_F3_UP,//0xBD
        KEY_F4_UP,//0xBE
        KEY_F5_UP,//0xBF
        KEY_F6_UP,//0xC0
        KEY_F7_UP,//0xC1
        KEY_F8_UP,//0xC2
        KEY_F9_UP,//0xC3
        KEY_F10_UP,//0xC4
        KEY_NUMLOCK_UP,//0xC5
        KEY_SCROLL_LOCK_UP,//0xC6
        KEY_KP_7_UP,//0xC7
        KEY_KP_8_UP,//0xC8
        KEY_KP_9_UP,//0xC9
        KEY_KP_MINUS_UP,//0xCA
        KEY_KP_4_UP,//0xCB
        KEY_KP_5_UP,//0xCC
        KEY_KP_6_UP,//0xCD
        KEY_KP_PLUS_UP,//0xCE
        KEY_KP_1_UP,//0xCF
        KEY_KP_2_UP,//0xD0
        KEY_KP_3_UP,//0xD1
        KEY_KP_0_UP,//0xD2
        KEY_KP_DOT_UP,//0xD3
        KEY_INVALID,//0xD4
        KEY_INVALID,//0xD5
        KEY_INVALID,//0xD6
        KEY_F11_UP,//0xD7
        KEY_F12_UP,//0xD8
        KEY_INVALID,//0xD9
        KEY_INVALID,//0xDA
        KEY_INVALID,//0xDB
        KEY_INVALID,//0xDC
        KEY_INVALID,//0xDD
        KEY_INVALID,//0xDE
        KEY_INVALID,//0xDF
        KEY_EXTRA,//0xE0
    };
    constexpr const uint16_t scancodes_us_qwerty_set1_extra[] = {
    };
}

#endif // KEYBOARD_H_INCLUDED
