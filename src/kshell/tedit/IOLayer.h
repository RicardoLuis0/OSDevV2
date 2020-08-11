#pragma once

#include <stdint.h>
#include <stddef.h>

namespace TEdit::IOLayer {
    void init();
    enum color {
        BLACK = 0,
        BLUE = 1,
        GREEN = 2,
        CYAN = 3,
        RED = 4,
        MAGENTA = 5,
        BROWN = 6,
        LIGHT_GREY = 7,
        DARK_GREY = 8,
        LIGHT_BLUE = 9,
        LIGHT_GREEN = 10,
        LIGHT_CYAN = 11,
        LIGHT_RED = 12,
        LIGHT_MAGENTA = 13,
        LIGHT_BROWN = 14,
        WHITE = 15,
    };
    enum key_type {
        ARROW_LEFT,
        ARROW_UP,
        ARROW_RIGHT,
        ARROW_DOWN,
        ALT,
        KEY,
    };
    struct keypress {
        char key;
        uint8_t type;
        uint16_t count;
    };
    keypress get_key();
    void moveCursor(int16_t x,int16_t y);
    void writeChar(char c);
    void writeChars(const char * s,size_t n);
    void writeStr(const char * s);
    void setColor(color fg,color bg);
    void fillLine(int16_t line,int16_t rows,char c,color fg,color bg);
    [[noreturn]] void exit_error(const char * fmt,...);
};
