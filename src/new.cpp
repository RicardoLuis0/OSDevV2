#include "new.h"
#include "klib.h"

void * operator new(size_t size) {
    return k_malloc(size);
}

void * operator new[](size_t size) {
    return k_malloc(size);
}

void operator delete(void *p) {
    k_free(p);
}

void operator delete[](void *p) {
    k_free(p);
}
