#include "kui.h"
#include "print.h"
#include "tdraw.h"

void KUI::kui(){
    TextDraw::drawFilledBox(Screen::WHITE,{0,0,80,25});
    TextDraw::drawLineBox(Screen::DARK_GREY,{0,0,80,1});
    Screen::move(0,0);
    Screen::setcolor(Screen::LIGHT_GREY,Screen::WHITE);
    Screen::write_s_skip_space("Help Quit");
}
