#include <klib.h>
extern "C" {

    int abs(int i){
        return i>0?i:-i;
    }

    double fabs(double f){
        return f>0?f:-f;
    }

    float fabsf(float f){
        return f>0?f:-f;
    }

}
