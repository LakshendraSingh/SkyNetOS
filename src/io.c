#include <stdint.h>
#include "io.h"
#include "error_codes.h"
#include "scheduler.h"

/* GUI output redirection */
bool gui_output_mode = false;
static void (*gui_print_fn)(const char*) = 0;
static void (*gui_putchar_fn)(char) = 0;

void io_set_gui_output(void (*print_fn)(const char*), void (*putchar_fn)(char)) {
    gui_print_fn = print_fn;
    gui_putchar_fn = putchar_fn;
}

#ifdef __i386__
void init_serial() {
   outb(0x3f8 + 1, 0x00);
   outb(0x3f8 + 3, 0x80);
   outb(0x3f8 + 0, 0x03);
   outb(0x3f8 + 1, 0x00);
   outb(0x3f8 + 3, 0x03);
   outb(0x3f8 + 2, 0xC7);
   outb(0x3f8 + 4, 0x0B);
}

void serial_putchar(char c) {
   outb(0x3f8, c);
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a" (ret) : "Nd" (port));
    return ret;
}
#else
void init_serial() {}
void serial_putchar(char c) { (void)c; }
static inline uint8_t inb(uint16_t port) {
    (void)port;
    return 0; // Mock for hosted tests
}
#endif


// Static screen state
static uint16_t cursor_pos = 0;
#ifdef __i386__
static volatile char *video_memory = (volatile char *)0xB8000;
#else
static char mock_video_memory[80 * 25 * 2];
static volatile char *video_memory = (volatile char *)mock_video_memory;
#endif

#define SCREEN_COLS 80
#define SCREEN_ROWS 25

// Scan code to ASCII table (US QWERTY)
static const char keymap[] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',
  '9', '0', '-', '=', '\b',
  '\t',
  'q', 'w', 'e', 'r',
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
 '\'', '`',   0,
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',
  'm', ',', '.', '/',   0,
  '*',
    0,
  ' ',
    0,
    0,
    0,   0,   0,   0,   0,   0,   0,   0,
    0,
    0,
    0,
    0,
    0,
    0,
  '-',
    0,
    0,
    0,
  '+',
    0,
    0,
    0,
    0,
    0,   0,   0,
    0,
    0,
    0,
};

// Shifted keymap (Shift + key)
static const char keymap_shift[] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*',
  '(', ')', '_', '+', '\b',
  '\t',
  'Q', 'W', 'E', 'R',
  'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0,
  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',
  '"', '~',   0,
  '|', 'Z', 'X', 'C', 'V', 'B', 'N',
  'M', '<', '>', '?',   0,
  '*',
    0,
  ' ',
    0,
    0,
    0,   0,   0,   0,   0,   0,   0,   0,
    0,
    0,
    0,
    0,
    0,
    0,
  '-',
    0,
    0,
    0,
  '+',
    0,
    0,
    0,
    0,
    0,   0,   0,
    0,
    0,
    0,
};

// Shift key state
static int shift_held = 0;

// Scroll the screen up by one line
static void scroll_screen() {
    // Move all rows up by one
    for (int i = 0; i < (SCREEN_ROWS - 1) * SCREEN_COLS * 2; i++) {
        video_memory[i] = video_memory[i + SCREEN_COLS * 2];
    }
    // Clear the last row
    for (int i = (SCREEN_ROWS - 1) * SCREEN_COLS * 2; i < SCREEN_ROWS * SCREEN_COLS * 2; i += 2) {
        video_memory[i] = ' ';
        video_memory[i + 1] = 0x07;
    }
}

void print_char(char c) {
    serial_putchar(c);
    if (c == '\n') {
        cursor_pos += SCREEN_COLS - (cursor_pos % SCREEN_COLS);
    } else if (c == '\b') {
        if (cursor_pos > 0) {
            cursor_pos--;
            video_memory[cursor_pos * 2] = ' ';
            video_memory[cursor_pos * 2 + 1] = 0x07;
        }
    } else if (c == '\t') {
        // Tab = advance to next 8-column boundary
        cursor_pos += 8 - (cursor_pos % 8);
    } else if ((unsigned char)c >= 32 && (unsigned char)c != 127) {
        video_memory[cursor_pos * 2] = c;
        video_memory[cursor_pos * 2 + 1] = 0x07;
        cursor_pos++;
    }

    // Scroll if we reach the end of the screen
    while (cursor_pos >= SCREEN_COLS * SCREEN_ROWS) {
        scroll_screen();
        cursor_pos -= SCREEN_COLS;
    }
}

void print(const char* str) {
    if (gui_output_mode && gui_print_fn) {
        gui_print_fn(str);
        return;
    }
    while (*str) {
        unsigned char c = (unsigned char)*str;
        // Convert 3-byte UTF-8 box-drawing/block chars to CP437
        if (c == 0xE2 && str[1] && str[2]) {
            unsigned char b1 = (unsigned char)str[1];
            unsigned char b2 = (unsigned char)str[2];
            char cp = 0;
            if (b1 == 0x95) {
                switch (b2) {
                    case 0x90: cp = (char)0xCD; break; // ═
                    case 0x91: cp = (char)0xBA; break; // ║
                    case 0x94: cp = (char)0xC9; break; // ╔
                    case 0x97: cp = (char)0xBB; break; // ╗
                    case 0x9A: cp = (char)0xC8; break; // ╚
                    case 0x9D: cp = (char)0xBC; break; // ╝
                    case 0xA0: cp = (char)0xCC; break; // ╠
                    case 0xA3: cp = (char)0xB9; break; // ╣
                }
            } else if (b1 == 0x96 && b2 == 0x88) {
                cp = (char)0xDB; // █
            } else if (b1 == 0x80 && b2 == 0x94) {
                cp = '-'; // — (em dash)
            }
            if (cp) {
                print_char(cp);
                str += 3;
                continue;
            }
        }
        // Skip unrecognized multi-byte UTF-8 sequences
        if (c >= 0xE0 && c <= 0xEF && str[1] && str[2]) {
            str += 3;
            continue;
        }
        if (c >= 0xC0 && c <= 0xDF && str[1]) {
            str += 2;
            continue;
        }
        if (c >= 0xF0 && c <= 0xF7 && str[1] && str[2] && str[3]) {
            str += 4;
            continue;
        }
        print_char(*str++);
    }
}

void print_int(int n) {
    if (gui_output_mode && gui_putchar_fn) {
        if (n == 0) { gui_putchar_fn('0'); return; }
        if (n < 0) { gui_putchar_fn('-'); n = -n; }
        char buf[12]; int i = 0;
        while (n > 0) { buf[i++] = '0' + (n % 10); n /= 10; }
        while (--i >= 0) gui_putchar_fn(buf[i]);
        return;
    }
    if (n == 0) {
        print_char('0');
        return;
    }
    if (n < 0) {
        print_char('-');
        n = -n;
    }
    char buf[12];
    int i = 0;
    while (n > 0) {
        buf[i++] = (n % 10) + '0';
        n /= 10;
    }
    while (--i >= 0) {
        print_char(buf[i]);
    }
}

void clear_screen() {
    for (int i = 0; i < SCREEN_COLS * SCREEN_ROWS * 2; i += 2) {
        video_memory[i] = ' ';
        video_memory[i + 1] = 0x07;
    }
    cursor_pos = 0;
}

char read_char() {
    int tick_counter = 0;
    while (1) {
        while (!(inb(0x64) & 0x01)) {
            tick_counter++;
            if (tick_counter > 500000) {
                system_tick();
                tick_counter = 0;
            }
        }
        uint8_t scancode = inb(0x60);

        // Left Shift press/release (0x2A / 0xAA)
        // Right Shift press/release (0x36 / 0xB6)
        if (scancode == 0x2A || scancode == 0x36) {
            shift_held = 1;
            continue;
        }
        if (scancode == 0xAA || scancode == 0xB6) {
            shift_held = 0;
            continue;
        }

        // Ignore key releases
        if (scancode & 0x80) continue;

        if (scancode < sizeof(keymap)) {
            char c = shift_held ? keymap_shift[scancode] : keymap[scancode];
            if (c) return c;
        }
    }
}

void read_line(char* buffer, int max_len) {
    int i = 0;
    while (i < max_len - 1) {
        char c = read_char();
        if (c == 0) continue; 
        
        if (c == '\n') {
            buffer[i] = '\0';
            print_char('\n');
            break;
        } else if (c == '\b') {
            if (i > 0) {
                i--;
                print_char('\b');
            }
        } else if (c >= 32 && c <= 126) {
            buffer[i++] = c;
            print_char(c);
        }
    }
    buffer[i] = '\0';
}

void read_line_masked(char* buffer, int max_len) {
    int i = 0;
    while (i < max_len - 1) {
        char c = read_char();
        if (c == 0) continue;

        if (c == '\n') {
            buffer[i] = '\0';
            print_char('\n');
            break;
        } else if (c == '\b') {
            if (i > 0) {
                i--;
                print_char('\b');
            }
        } else if (c >= 32 && c <= 126) {
            buffer[i++] = c;
            print_char('*'); // Mask with asterisk
        }
    }
    buffer[i] = '\0';
}

/* ═══════════════ PS/2 Mouse Driver ═══════════════ */

#ifdef __i386__
static int mouse_cycle = 0;
static uint8_t mouse_packet[3];

static void mouse_wait_write(void) {
    for (int t = 100000; t > 0; t--)
        if (!(inb(0x64) & 2)) return;
}

static void mouse_wait_read(void) {
    for (int t = 100000; t > 0; t--)
        if (inb(0x64) & 1) return;
}

static void mouse_cmd(uint8_t cmd) {
    mouse_wait_write();
    outb(0x64, 0xD4);     // Next byte goes to mouse
    mouse_wait_write();
    outb(0x60, cmd);
}
#endif

void mouse_init(void) {
#ifdef __i386__
    mouse_cycle = 0;

    // Enable auxiliary device
    mouse_wait_write();
    outb(0x64, 0xA8);

    // Get controller config, enable IRQ12 + mouse clock
    mouse_wait_write();
    outb(0x64, 0x20);
    mouse_wait_read();
    uint8_t status = inb(0x60);
    status |= 2;       // Enable IRQ12
    status &= ~0x20;   // Enable mouse clock
    mouse_wait_write();
    outb(0x64, 0x60);
    mouse_wait_write();
    outb(0x60, status);

    // Default settings + enable data reporting
    mouse_cmd(0xF6);
    mouse_wait_read(); inb(0x60);  // ACK
    mouse_cmd(0xF4);
    mouse_wait_read(); inb(0x60);  // ACK
#endif
}

void mouse_disable(void) {
#ifdef __i386__
    mouse_cmd(0xF5);               // Disable data reporting
    mouse_wait_read(); inb(0x60);  // ACK
#endif
}

int input_poll(uint8_t *scancode, int *mouse_dx, int *mouse_dy, int *mouse_buttons) {
#ifdef __i386__
    uint8_t st = inb(0x64);
    if (!(st & 0x01)) return INPUT_NONE;  // No data

    uint8_t data = inb(0x60);

    if (st & 0x20) {
        // Auxiliary (mouse) data
        switch (mouse_cycle) {
            case 0:
                if (data & 0x08) {        // Sync bit must be set
                    mouse_packet[0] = data;
                    mouse_cycle = 1;
                }
                break;
            case 1:
                mouse_packet[1] = data;
                mouse_cycle = 2;
                break;
            case 2:
                mouse_packet[2] = data;
                mouse_cycle = 0;

                *mouse_buttons = mouse_packet[0] & 0x07;
                int dx = mouse_packet[1];
                int dy = mouse_packet[2];
                if (mouse_packet[0] & 0x10) dx |= 0xFFFFFF00;  // Sign-extend X
                if (mouse_packet[0] & 0x20) dy |= 0xFFFFFF00;  // Sign-extend Y
                if (mouse_packet[0] & 0xC0) { dx = 0; dy = 0; } // Overflow → ignore
                *mouse_dx = dx;
                *mouse_dy = -dy;  // Screen Y is inverted
                return INPUT_MOUSE;
        }
        return INPUT_NONE;  // Partial packet, not ready yet
    } else {
        // Keyboard scancode
        *scancode = data;
        return INPUT_KEYBOARD;
    }
#else
    (void)scancode; (void)mouse_dx; (void)mouse_dy; (void)mouse_buttons;
    return INPUT_NONE;
#endif
}
