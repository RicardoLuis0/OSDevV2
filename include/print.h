#ifndef PRINT_H_INCLUDED
#define PRINT_H_INCLUDED

#include "screen.h"

template <typename T> void print(T);

template<> void print(char c);

template<> void print(int i);

template<> void print(unsigned int i);

template<> void print(const char * s);

template<> void print(char * s);

template<typename T,typename... Tn> void print(T arg0,Tn ... args){
    print(arg0);
    (print(args),...);
}

#endif // PRINT_H_INCLUDED
