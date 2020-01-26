#ifndef CPUID_H_INCLUDED
#define CPUID_H_INCLUDED

#include "stdc/stdint.h"

extern "C" uint32_t has_cpuid();

namespace CPUID {
    void check();
}

#endif // CPUID_H_INCLUDED
