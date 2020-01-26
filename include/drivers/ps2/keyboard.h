#ifndef KEYBOARD_H_INCLUDED
#define KEYBOARD_H_INCLUDED

#include "stdc/stdint.h"

namespace Drivers{
namespace PS2{
namespace Keyboard {
    enum keycode{
        KEY_INVALID=0,
        KEY_0_PRESSED='0',
        KEY_1_PRESSED,
        KEY_2_PRESSED,
        KEY_3_PRESSED,
        KEY_4_PRESSED,
        KEY_5_PRESSED,
        KEY_6_PRESSED,
        KEY_7_PRESSED,
        KEY_8_PRESSED,
        KEY_9_PRESSED,
        KEY_A_PRESSED='a',
        KEY_B_PRESSED,
        KEY_C_PRESSED,
        KEY_D_PRESSED,
        KEY_E_PRESSED,
        KEY_F_PRESSED,
        KEY_G_PRESSED,
        KEY_H_PRESSED,
        KEY_I_PRESSED,
        KEY_J_PRESSED,
        KEY_K_PRESSED,
        KEY_L_PRESSED,
        KEY_M_PRESSED,
        KEY_N_PRESSED,
        KEY_O_PRESSED,
        KEY_P_PRESSED,
        KEY_Q_PRESSED,
        KEY_R_PRESSED,
        KEY_S_PRESSED,
        KEY_T_PRESSED,
        KEY_U_PRESSED,
        KEY_V_PRESSED,
        KEY_W_PRESSED,
        KEY_X_PRESSED,
        KEY_Y_PRESSED,
        KEY_Z_PRESSED,
        KEY_LEFT_BRACKET_PRESSED='[',
        KEY_RIGHT_BRACKET_PRESSED=']',
        KEY_MINUS_PRESSED='-',
        KEY_EQUALS_PRESSED='=',
        KEY_ESC_PRESSED=27,
        KEY_BACKSPACE_PRESSED='\b',
        KEY_TAB_PRESSED='\t',
        KEY_ENTER_PRESSED='\n',
        KEY_SEMICOLON_PRESSED=';',
        KEY_SINGLE_QUOTE_PRESSED='\'',
        KEY_BACKTICK_PRESSED='`',
        KEY_BACKSLASH_PRESSED='\\',
        KEY_SPACE_PRESSED=' ',
        KEY_COMMA_PRESSED=',',
        KEY_DOT_PRESSED='.',
        KEY_SLASH_PRESSED='/',
        SCANCODE_SPECIAL=256,//after this only non-ascii
        KEY_LEFT_SHIFT_PRESSED,
        KEY_RIGHT_SHIFT_PRESSED,
        KEY_LEFT_CONTROL_PRESSED,
        KEY_LEFT_ALT_PRESSED,
        KEY_CAPSLOCK_PRESSED,
        KEY_NUM_LOCK_PRESSED,
        KEY_SCROLL_LOCK_PRESSED,
        KEY_F1_PRESSED,
        KEY_F2_PRESSED,
        KEY_F3_PRESSED,
        KEY_F4_PRESSED,
        KEY_F5_PRESSED,
        KEY_F6_PRESSED,
        KEY_F7_PRESSED,
        KEY_F8_PRESSED,
        KEY_F9_PRESSED,
        KEY_F10_PRESSED,
        KEY_F11_PRESSED,
        KEY_F12_PRESSED,
        KEY_KP_STAR_PRESSED,
        KEY_KP_MINUS_PRESSED,
        KEY_KP_PLUS_PRESSED,
        KEY_KP_DOT_PRESSED,
        KEY_KP_0_PRESSED,
        KEY_KP_1_PRESSED,
        KEY_KP_2_PRESSED,
        KEY_KP_3_PRESSED,
        KEY_KP_4_PRESSED,
        KEY_KP_5_PRESSED,
        KEY_KP_6_PRESSED,
        KEY_KP_7_PRESSED,
        KEY_KP_8_PRESSED,
        KEY_KP_9_PRESSED,
        //
        KEY_0_RELEASED,
        KEY_1_RELEASED,
        KEY_2_RELEASED,
        KEY_3_RELEASED,
        KEY_4_RELEASED,
        KEY_5_RELEASED,
        KEY_6_RELEASED,
        KEY_7_RELEASED,
        KEY_8_RELEASED,
        KEY_9_RELEASED,
        KEY_A_RELEASED,
        KEY_B_RELEASED,
        KEY_C_RELEASED,
        KEY_D_RELEASED,
        KEY_E_RELEASED,
        KEY_F_RELEASED,
        KEY_G_RELEASED,
        KEY_H_RELEASED,
        KEY_I_RELEASED,
        KEY_J_RELEASED,
        KEY_K_RELEASED,
        KEY_L_RELEASED,
        KEY_M_RELEASED,
        KEY_N_RELEASED,
        KEY_O_RELEASED,
        KEY_P_RELEASED,
        KEY_Q_RELEASED,
        KEY_R_RELEASED,
        KEY_S_RELEASED,
        KEY_T_RELEASED,
        KEY_U_RELEASED,
        KEY_V_RELEASED,
        KEY_W_RELEASED,
        KEY_X_RELEASED,
        KEY_Y_RELEASED,
        KEY_Z_RELEASED,
        KEY_LEFT_BRACKET_RELEASED,
        KEY_RIGHT_BRACKET_RELEASED,
        KEY_MINUS_RELEASED,
        KEY_EQUALS_RELEASED,
        KEY_ESC_RELEASED,
        KEY_BACKSPACE_RELEASED,
        KEY_TAB_RELEASED,
        KEY_ENTER_RELEASED,
        KEY_SEMICOLON_RELEASED,
        KEY_SINGLE_QUOTE_RELEASED,
        KEY_BACKTICK_RELEASED,
        KEY_BACKSLASH_RELEASED,
        KEY_SPACE_RELEASED,
        KEY_COMMA_RELEASED,
        KEY_DOT_RELEASED,
        KEY_SLASH_RELEASED,
        KEY_LEFT_SHIFT_RELEASED,
        KEY_RIGHT_SHIFT_RELEASED,
        KEY_LEFT_CONTROL_RELEASED,
        KEY_LEFT_ALT_RELEASED,
        KEY_CAPSLOCK_RELEASED,
        KEY_NUM_LOCK_RELEASED,
        KEY_SCROLL_LOCK_RELEASED,
        KEY_F1_RELEASED,
        KEY_F2_RELEASED,
        KEY_F3_RELEASED,
        KEY_F4_RELEASED,
        KEY_F5_RELEASED,
        KEY_F6_RELEASED,
        KEY_F7_RELEASED,
        KEY_F8_RELEASED,
        KEY_F9_RELEASED,
        KEY_F10_RELEASED,
        KEY_F11_RELEASED,
        KEY_F12_RELEASED,
        KEY_KP_STAR_RELEASED,
        KEY_KP_MINUS_RELEASED,
        KEY_KP_PLUS_RELEASED,
        KEY_KP_DOT_RELEASED,
        KEY_KP_0_RELEASED,
        KEY_KP_1_RELEASED,
        KEY_KP_2_RELEASED,
        KEY_KP_3_RELEASED,
        KEY_KP_4_RELEASED,
        KEY_KP_5_RELEASED,
        KEY_KP_6_RELEASED,
        KEY_KP_7_RELEASED,
        KEY_KP_8_RELEASED,
        KEY_KP_9_RELEASED,
        //
        KEY_KP_ENTER_PRESSED,
        KEY_RIGHT_CONTROL_PRESSED,
        KEY_KP_SLASH_PRESSED,
        KEY_RIGHT_ALT_PRESSED,
        KEY_HOME_PRESSED,
        KEY_UP_PRESSED,
        KEY_PGUP_PRESSED,
        KEY_LEFT_PRESSED,
        KEY_RIGHT_PRESSED,
        KEY_END_PRESSED,
        KEY_DOWN_PRESSED,
        KEY_PGDOWN_PRESSED,
        KEY_INSERT_PRESSED,
        KEY_DELETE_PRESSED,
        KEY_LEFT_CMD_PRESSED,
        KEY_RIGHT_CMD_PRESSED,
        KEY_APPS_PRESSED,//???
        KEY_ACPI_POWER_PRESSED,
        KEY_ACPI_SLEEP_PRESSED,
        KEY_ACPI_WAKE_PRESSED,
        //
        KEY_KP_ENTER_RELEASED,
        KEY_RIGHT_CONTROL_RELEASED,
        KEY_KP_SLASH_RELEASED,
        KEY_RIGHT_ALT_RELEASED,
        KEY_HOME_RELEASED,
        KEY_UP_RELEASED,
        KEY_PGUP_RELEASED,
        KEY_LEFT_RELEASED,
        KEY_RIGHT_RELEASED,
        KEY_END_RELEASED,
        KEY_DOWN_RELEASED,
        KEY_PGDOWN_RELEASED,
        KEY_INSERT_RELEASED,
        KEY_DELETE_RELEASED,
        KEY_LEFT_CMD_RELEASED,
        KEY_RIGHT_CMD_RELEASED,
        KEY_APPS_RELEASED,//???
        KEY_ACPI_POWER_RELEASED,
        KEY_ACPI_SLEEP_RELEASED,
        KEY_ACPI_WAKE_RELEASED,
        //
        KEY_PAUSE,
        KEY_PRINTSCR_PRESSED,
        KEY_PRINTSCR_RELEASED,
    };
    void init();
    bool hasKey();
    keycode getKey();
    const char * keycode_name(keycode c);
}}}

#endif // KEYBOARD_H_INCLUDED
