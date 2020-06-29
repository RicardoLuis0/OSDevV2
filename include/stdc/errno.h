#ifndef ERRNO_H_INCLUDED
#define ERRNO_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern int errno;

char * strerror(int errnum);

enum errno_t{
    ERRNO_NONE,
    ERANGE,
};

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // ERRNO_H_INCLUDED
