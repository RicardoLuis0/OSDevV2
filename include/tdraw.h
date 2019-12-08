#ifndef TDRAW_H_INCLUDED
#define TDRAW_H_INCLUDED

#include "screen.h"

namespace TextDraw{
    struct box{
        int x,y,width,height;
    };
    void drawLineBox(Screen::color c,box bounds);
    void drawFilledBox(Screen::color c,box bounds);
    void drawFilledOutlineBox(Screen::color outline_c,Screen::color fill_c,box bounds);
}

#endif // TDRAW_H_INCLUDED
