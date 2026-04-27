#ifndef IO_H
#define IO_H

#include <stdint.h>
#include <stdbool.h>

void print_char(char c);
void print(const char* str);
void print_int(int n);
void read_line(char* buffer, int max_len);
void read_line_masked(char* buffer, int max_len);
char read_char();
void clear_screen();
void init_serial();

/* GUI output redirection — when true, print() → GUI terminal */
extern bool gui_output_mode;
void io_set_gui_output(void (*print_fn)(const char*), void (*putchar_fn)(char));

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

/* PS/2 Mouse support */
void mouse_init(void);
void mouse_disable(void);

/* Non-blocking input poll for GUI event loop.
 * Returns: 0=nothing, 1=keyboard scancode, 2=complete mouse packet */
#define INPUT_NONE     0
#define INPUT_KEYBOARD 1
#define INPUT_MOUSE    2
int input_poll(uint8_t *scancode, int *mouse_dx, int *mouse_dy, int *mouse_buttons);

#endif
