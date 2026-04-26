#ifndef GUI_H
#define GUI_H

#include "vga.h"
#include <stdbool.h>

#define MAX_WINDOWS 8

/* Terminal buffer dimensions */
#define TERM_COLS 52      /* chars per line (fits ~430px window) */
#define TERM_ROWS 26      /* visible lines */
#define TERM_BUF_ROWS 40  /* scrollback */
#define TERM_INPUT_MAX 80  /* max input line length */

typedef enum {
    WIN_GENERIC,
    WIN_SYSMON,
    WIN_ABOUT,
    WIN_TERMINAL
} window_type_t;

typedef struct {
    int x, y, w, h;
    color_t bg_color;
    char title[32];
    bool active;
    window_type_t type;
} window_t;

typedef struct {
    window_t windows[MAX_WINDOWS];
    int window_count;
    int z_order[MAX_WINDOWS];   /* draw order: z_order[0] = back, last = front */
    int focused;                /* index of focused window, -1 = none */
    color_t desktop_color;
} gui_t;

void gui_init(void);
void gui_draw_desktop(void);
void gui_create_window(const char* title, int x, int y, int w, int h,
                        color_t color, window_type_t type);
void gui_draw_window(window_t* win);
void gui_render(void);

/* Basic text in graphics mode */
void gui_draw_char(int x, int y, char c, color_t color);
void gui_draw_string(int x, int y, const char* str, color_t color);
void gui_draw_int(int x, int y, int n, color_t color);

/* Window content renderers */
void gui_draw_sysmon_content(window_t* win);
void gui_draw_about_content(window_t* win);
void gui_draw_terminal_content(window_t* win);

/* Mouse cursor + interactive event loop */
void gui_draw_cursor(int x, int y);
void gui_event_loop(void);

/* Terminal output API (called from redirected print) */
void gui_term_putchar(char c);
void gui_term_print(const char* str);

#endif
