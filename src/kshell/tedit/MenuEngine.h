#pragma once

#include <stdint.h>

#include "IOLayer.h"

namespace TEdit::MenuEngine {
    void init();
    int16_t getHeight();
    void draw(bool focused);
    bool handle_input(IOLayer::keypress key);
    void setfile(const char * c);
    bool is_fullscreen();
    void quit();
};
