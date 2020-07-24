#ifndef TASKING_INTERNAL_H_INCLUDED
#define TASKING_INTERNAL_H_INCLUDED

#include "arch/x86.h"

namespace Tasking {
    struct task_data {
        CPU::CpuState cpu;
        //TODO
    };
}

#endif // TASKING_INTERNAL_H_INCLUDED
