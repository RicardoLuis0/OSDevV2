#include "klib.h"
#include <stdio.h>
#include <errno.h>
#include <locale.h>
extern "C" {
    lconv loc{.decimal_point=const_cast<char*>(".")};
    lconv * localeconv(){
        return &loc;
    }
    char * setlocale(int category, const char * locale){
        k_abort_s("setlocale unimplemented");
    }
}
