#ifndef X86_MSR_H_INCLUDED
#define X86_MSR_H_INCLUDED

namespace MSR {
    
    void get(uint32_t msr, uint32_t *lo, uint32_t *hi);
    void set(uint32_t msr, uint32_t lo, uint32_t hi);
    void get(uint32_t msr, uint64_t *v);
    uint64_t get(uint32_t msr);
    void set(uint32_t msr, uint64_t v);
    
}

#endif // X86_MSR_H_INCLUDED
