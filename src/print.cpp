#include "print.h"

template<> void print(char c){
    Screen::write_c(c);
}

template<> void print(int i){
    Screen::write_i(i);
}

template<> void print(unsigned int u){
    Screen::write_u(u);
}

template<> void print(const char * s){
    Screen::write_s(s);
}
