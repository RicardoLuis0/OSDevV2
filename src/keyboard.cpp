#include "keyboard.h"
#include "klib.h"

extern "C" char k_getchar(){
    k_abort_s("keyboard input not implemented");
    return 0;
}
