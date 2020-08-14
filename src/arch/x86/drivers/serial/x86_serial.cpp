#include "arch/x86.h"
#include "serial.h"
#include "klib.h"

#define PORT 0x3f8   /* COM1 */

//from https://wiki.osdev.org/Serial_Ports

void Serial::x86_init(){
    outb(PORT + 1, 0x00);    // Disable all interrupts
    outb(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    outb(PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
    outb(PORT + 1, 0x00);    //                  (hi byte)
    outb(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
    outb(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    outb(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}

int serial_received() {
    return inb(PORT + 5) & 1;
}

char read_serial() {
    while (serial_received() == 0);
    return inb(PORT);
}

int is_transmit_empty() {
    return inb(PORT + 5) & 0x20;
}

static void write_serial(char a) {
    while (is_transmit_empty() == 0);
    outb(PORT,a);
}

void Serial::write_s(const char * s){
    #ifdef SERIAL_CRLF
    char pc=0;
    while(char c=*s++){
        if(c=='\n'&&pc!='\r'&&s[1]!='\r')write_serial('\r');//convert LF not followed or preceded by CR into CRLF
        write_serial(c);
        pc=c;
    }
    #else
    while(char c=*s++)write_serial(c);
    #endif // SERIAL_CRLF
}

static void write_h_rec(uint64_t h){
    if(h>15){
        write_h_rec(h/16);
        h%=16;
    }
    write_serial((h<10)?(h+'0'):((h-10)+'A'));
}

void Serial::write_h(uint64_t h){
    write_s("0x");
    write_h_rec(h);
}
