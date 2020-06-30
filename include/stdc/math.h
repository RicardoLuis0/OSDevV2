#ifndef MATH_H_INCLUDED
#define MATH_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define HUGE_VAL    __builtin_huge_val()

typedef float float_t;
typedef double double_t;

double pow(double x, double y);
float powf(float x, float y);
double fabs(double f);
float fabsf(float f);
double sqrt(double arg);
float sqrtf(float arg);
float scalbnf(float arg, int exp);
double scalbn(double arg, int exp);

//TODO



#ifdef __cplusplus
}
#endif // __cplusplus

#endif // MATH_H_INCLUDED
