#ifndef SPINLOCK_H_INCLUDED
#define SPINLOCK_H_INCLUDED

namespace Util {

    class Spinlock {
        volatile int _lock;
    public:
        void lock();
        void release();
    };

    class SpinlockGuard{
        Spinlock&l;
    public:
        inline SpinlockGuard(Spinlock&l2):l(l2){
            l.lock();
        }
        inline ~SpinlockGuard(){
            l.release();
        }
    };

}

#endif // SPINLOCK_H_INCLUDED
