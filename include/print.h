#ifndef PRINT_H_INCLUDED
#define PRINT_H_INCLUDED

#include "screen.h"

template <typename T> void print(T);

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

template<typename T,typename... Tn> void print(T arg0,Tn ... args){
    print(arg0);
    (print(args),...);
}

#endif // PRINT_H_INCLUDED
