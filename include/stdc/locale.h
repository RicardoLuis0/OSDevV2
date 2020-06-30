#ifndef LOCALE_H_INCLUDED
#define LOCALE_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

struct lconv {
    char * decimal_point;
};

struct lconv * localeconv();

//TODO

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // LOCALE_H_INCLUDED
