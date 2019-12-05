#ifndef SCREEN_H_INCLUDED
#define SCREEN_H_INCLUDED

#include "default/stdint.h"

namespace Screen {
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
    void init();
    void newline();
    void move(int x,int y);
    void move(int pos);
    void setchar(char c);
    void write_c(char c);
    void write_s(const char * str);
    void write_i(unsigned int i);
    void write_h(unsigned int h);//write int as hex
    void write_mem(uint64_t mem);//write as memory size
    void setcolor(color bg,color fg);
    void setbgcolor(color c);
    void setfgcolor(color c);
    void clear();
}

#endif // SCREEN_H_INCLUDED
