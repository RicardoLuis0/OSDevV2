#include "print.h"

template<> void print(char c){
    Screen::write_c(c);
}

template<> void print(int i){
    if(i<0){
        Screen::write_c('-');
        i*=-1;
    }
    Screen::write_i(i);
}

template<> void print(unsigned int i){
    Screen::write_i(i);
}

template<> void print(const char * s){
    Screen::write_s(s);
}
