#include "klib.h"
#include <stdio.h>
#include <errno.h>
#include <locale.h>
extern "C" {
    lconv loc{.decimal_point=(char*)"."};
    lconv * localeconv(){
        return &loc;
    }
}
