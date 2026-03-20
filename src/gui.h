#ifndef GUI_H
#define GUI_H

#include "vga.h"
#include <stdbool.h>

#define MAX_WINDOWS 8

typedef struct {
    int x, y, w, h;
    color_t bg_color;
    char title[32];
    bool active;
} window_t;

typedef struct {
    window_t windows[MAX_WINDOWS];
    int window_count;
    color_t desktop_color;
} gui_t;

void gui_init();
void gui_draw_desktop();
void gui_create_window(const char* title, int x, int y, int w, int h, color_t color);
void gui_draw_window(window_t* win);
void gui_render();

// Basic text in graphics mode
void gui_draw_char(int x, int y, char c, color_t color);
void gui_draw_string(int x, int y, const char* str, color_t color);

#endif
