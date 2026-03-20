#ifndef VGA_H
#define VGA_H

#include <stdint.h>
#include "io.h"

#define VGA_WIDTH 800
#define VGA_HEIGHT 600

typedef uint8_t color_t;

void vga_set_mode_13h();
void vga_set_text_mode();
void vga_put_pixel(int x, int y, color_t color);
void vga_fill_rect(int x, int y, int w, int h, color_t color);
void vga_clear(color_t color);

#endif
