#include <stdint.h>
#include "io.h"
#include "error_codes.h"


#ifdef __i386__
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a" (ret) : "Nd" (port));
    return ret;
}
#else
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
    } else if (c >= 32 && c <= 126) {
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
    while (*str) {
        print_char(*str++);
    }
}

void print_int(int n) {
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
    while (1) {
        while (!(inb(0x64) & 0x01));
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
