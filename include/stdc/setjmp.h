#ifndef SETJMP_H_INCLUDED
#define SETJMP_H_INCLUDED


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef long jmp_buf[32];

#define longjmp(b,v) __builtin_longjmp((void**)&b,v)
#define setjmp(b) __builtin_setjmp((void**)&b)

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // SETJMP_H_INCLUDED
