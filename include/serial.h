#ifndef SERIAL_H_INCLUDED
#define SERIAL_H_INCLUDED

namespace Serial {
    void write_s(const char *);
    void write_h(uint64_t);
    void write_i(int);
    void write_u(unsigned int);
}

#endif // SERIAL_H_INCLUDED
