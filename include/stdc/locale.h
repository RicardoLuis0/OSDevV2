#ifndef LOCALE_H_INCLUDED
#define LOCALE_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

struct lconv {
    char * decimal_point;
};

struct lconv * localeconv();

enum locale_categories{
    LC_ALL,
    LC_COLLATE,
    LC_CTYPE,
    LC_MONETARY,
    LC_NUMERIC,
    LC_TIME,
};

char* setlocale(int category, const char * locale);

//TODO

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // LOCALE_H_INCLUDED
