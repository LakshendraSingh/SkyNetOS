#ifndef IO_H
#define IO_H

#include <stdint.h>

void print_char(char c);
void print(const char* str);
void print_int(int n);
void read_line(char* buffer, int max_len);
void read_line_masked(char* buffer, int max_len);
char read_char();
void clear_screen();

#ifdef __i386__
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a" (val), "Nd" (port));
}

static inline void outw(uint16_t port, uint16_t val) {
    __asm__ volatile ("outw %0, %1" : : "a" (val), "Nd" (port));
}
#else
static inline void outb(uint16_t port, uint8_t val) { (void)port; (void)val; }
static inline void outw(uint16_t port, uint16_t val) { (void)port; (void)val; }
#endif

#endif
