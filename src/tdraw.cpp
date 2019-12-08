#include "tdraw.h"

using namespace TextDraw;

void TextDraw::drawLineBox(Screen::color c,box bounds){
    Screen::color oldbg=Screen::getbgcolor();
    Screen::setbgcolor(c);
    int ylow=bounds.y+bounds.height;
    int xhigh=bounds.x+bounds.width;
    for(int i=bounds.x;i<xhigh;i++){
        Screen::move(i,bounds.y);
        Screen::write_c(' ');
        Screen::move(i,ylow-1);
        Screen::write_c(' ');
    }
    for(int i=bounds.y;i<ylow;i++){
        Screen::move(bounds.x,i);
        Screen::write_c(' ');
        Screen::move(xhigh-1,i);
        Screen::write_c(' ');
        
    }
    Screen::setbgcolor(oldbg);
}

void TextDraw::drawFilledBox(Screen::color c,box bounds){
    Screen::color oldbg=Screen::getbgcolor();
    Screen::setbgcolor(c);
    int ylow=bounds.y+bounds.height;
    int xhigh=bounds.x+bounds.width;
    for(int i=bounds.x;i<xhigh;i++){
        for(int j=bounds.y;j<ylow;j++){
            Screen::move(i,j);
            Screen::write_c(' ');
        }
    }
    Screen::setbgcolor(oldbg);
}

void TextDraw::drawFilledOutlineBox(Screen::color outline_c,Screen::color fill_c,box bounds){
    drawLineBox(outline_c,bounds);
    drawFilledBox(fill_c,{bounds.x+1,bounds.y+1,bounds.width-2,bounds.height-2});
}
