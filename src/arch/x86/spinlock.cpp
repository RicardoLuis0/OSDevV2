#include "util/spinlock.h"

using namespace Util;

void Spinlock::lock() {
    while(!__sync_bool_compare_and_swap(&_lock, 0, 1)) {
        asm("pause");
    }
}

bool Spinlock::try_lock() {
    return __sync_bool_compare_and_swap(&_lock, 0, 1);
}

void Spinlock::release() {
    _lock = 0;
}
