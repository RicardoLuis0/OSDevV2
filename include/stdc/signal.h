#ifndef SIGNAL_H_INCLUDED
#define SIGNAL_H_INCLUDED

#include <stdatomic.h>

#ifdef __cplusplus
extern "C" {
#define SIG_ERR (reinterpret_cast<void(*)(int)>(0))
#define SIG_DFL (reinterpret_cast<void(*)(int)>(1))
#define SIG_IGN (reinterpret_cast<void(*)(int)>(2))
#else
#define SIG_ERR ((void(*)(int))NULL)
#define SIG_DFL ((void(*)(int))1)
#define SIG_IGN ((void(*)(int))2)
#endif // __cplusplus

typedef atomic_int sig_atomic_t;

enum sig_types{
    SIGABRT,
    SIGFPE,
    SIGILL,
    SIGINT,
    SIGSEGV,
    SIGTERM,
};


void (*signal( int sig, void (*handler) (int))) (int);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // SIGNAL_H_INCLUDED
