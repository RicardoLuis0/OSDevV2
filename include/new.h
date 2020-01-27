#ifndef NEW_H_INCLUDED
#define NEW_H_INCLUDED

#include "stdc/stddef.h"

inline void * operator new(size_t, void *p) throw() { return p; }
inline void * operator new[](size_t, void *p) throw() { return p; }
inline void operator delete (void *, void *) throw() {};
inline void operator delete[](void *, void *) throw() {};

void * operator new(size_t size);
void * operator new[](size_t size);
void operator delete(void *p);
void operator delete[](void *p);

#endif // NEW_H_INCLUDED
