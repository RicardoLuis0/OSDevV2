#ifndef X86_CPU_H_INCLUDED
#define X86_CPU_H_INCLUDED

namespace CPU {
    class CpuState{
        //TODO implement
    public:
        void save();
        void new_at(void * code,void * stack, void * page_directory);
        [[noreturn]] void restore();
    };
}

#endif // X86_CPU_H_INCLUDED
