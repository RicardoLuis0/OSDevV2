#ifndef TIME_H_INCLUDED
#define TIME_H_INCLUDED

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef int64_t time_t;
typedef int64_t clock_t;

time_t time(time_t* timer);
clock_t clock(void);

struct tm {
    int tm_sec;   /* 0-60 */
    int tm_min;   /* 0-59 */
    int tm_hour;  /* 0-23 */
    int tm_mday;  /* 1-31 */
    int tm_mon;   /* 0-11 */
    int tm_year;  /* years since 1900 */
    int tm_wday;  /* 0-6 */
    int tm_yday;  /* 0-365 */
    int tm_isdst; /* >0 DST, 0 no DST, <0 information unavailable */
};

time_t mktime(struct tm *time);
struct tm * gmtime(const time_t *timer);
struct tm * localtime(const time_t *timer);

size_t strftime(char *s, size_t n,const char *fmt,const struct tm *);
double difftime(time_t time_end,time_t time_beg);

#define CLOCKS_PER_SEC 1

//TODO

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TIME_H_INCLUDED
