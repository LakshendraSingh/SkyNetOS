#include "gui.h"
#include "io.h"
#include "shell.h"
#include "string.h"

static gui_t system_gui;

// Complete 8x8 bitmap font for printable ASCII (32-126)
// Each character is 8 bytes (8 rows of 8 pixels).
// Index = (char - 32)
static const uint8_t font8x8[][8] = {
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //   (32)
    {0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x18, 0x00}, // ! (33)
    {0x6C, 0x6C, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00}, // " (34)
    {0x24, 0x7E, 0x24, 0x24, 0x7E, 0x24, 0x00, 0x00}, // # (35)
    {0x18, 0x3E, 0x60, 0x3C, 0x06, 0x7C, 0x18, 0x00}, // $ (36)
    {0x62, 0x64, 0x08, 0x10, 0x26, 0x46, 0x00, 0x00}, // % (37)
    {0x38, 0x44, 0x38, 0x3A, 0x44, 0x44, 0x3A, 0x00}, // & (38)
    {0x18, 0x18, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00}, // ' (39)
    {0x08, 0x10, 0x20, 0x20, 0x20, 0x10, 0x08, 0x00}, // ( (40)
    {0x20, 0x10, 0x08, 0x08, 0x08, 0x10, 0x20, 0x00}, // ) (41)
    {0x00, 0x24, 0x18, 0x7E, 0x18, 0x24, 0x00, 0x00}, // * (42)
    {0x00, 0x18, 0x18, 0x7E, 0x18, 0x18, 0x00, 0x00}, // + (43)
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x10}, // , (44)
    {0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00}, // - (45)
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00}, // . (46)
    {0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x00, 0x00}, // / (47)
    {0x3C, 0x42, 0x46, 0x5A, 0x62, 0x42, 0x3C, 0x00}, // 0 (48)
    {0x18, 0x38, 0x18, 0x18, 0x18, 0x18, 0x7E, 0x00}, // 1 (49)
    {0x3C, 0x42, 0x02, 0x0C, 0x30, 0x40, 0x7E, 0x00}, // 2 (50)
    {0x3C, 0x42, 0x02, 0x1C, 0x02, 0x42, 0x3C, 0x00}, // 3 (51)
    {0x0C, 0x14, 0x24, 0x44, 0x7E, 0x04, 0x04, 0x00}, // 4 (52)
    {0x7E, 0x40, 0x7C, 0x02, 0x02, 0x42, 0x3C, 0x00}, // 5 (53)
    {0x1C, 0x20, 0x40, 0x7C, 0x42, 0x42, 0x3C, 0x00}, // 6 (54)
    {0x7E, 0x02, 0x04, 0x08, 0x10, 0x10, 0x10, 0x00}, // 7 (55)
    {0x3C, 0x42, 0x42, 0x3C, 0x42, 0x42, 0x3C, 0x00}, // 8 (56)
    {0x3C, 0x42, 0x42, 0x3E, 0x02, 0x04, 0x38, 0x00}, // 9 (57)
    {0x00, 0x18, 0x18, 0x00, 0x18, 0x18, 0x00, 0x00}, // : (58)
    {0x00, 0x18, 0x18, 0x00, 0x18, 0x18, 0x10, 0x00}, // ; (59)
    {0x04, 0x08, 0x10, 0x20, 0x10, 0x08, 0x04, 0x00}, // < (60)
    {0x00, 0x00, 0x7E, 0x00, 0x7E, 0x00, 0x00, 0x00}, // = (61)
    {0x20, 0x10, 0x08, 0x04, 0x08, 0x10, 0x20, 0x00}, // > (62)
    {0x3C, 0x42, 0x04, 0x08, 0x10, 0x00, 0x10, 0x00}, // ? (63)
    {0x3C, 0x42, 0x4E, 0x52, 0x4E, 0x40, 0x3C, 0x00}, // @ (64)
    {0x18, 0x24, 0x42, 0x42, 0x7E, 0x42, 0x42, 0x00}, // A (65)
    {0x7C, 0x42, 0x42, 0x7C, 0x42, 0x42, 0x7C, 0x00}, // B (66)
    {0x3C, 0x42, 0x40, 0x40, 0x40, 0x42, 0x3C, 0x00}, // C (67)
    {0x78, 0x44, 0x42, 0x42, 0x42, 0x44, 0x78, 0x00}, // D (68)
    {0x7E, 0x40, 0x40, 0x7C, 0x40, 0x40, 0x7E, 0x00}, // E (69)
    {0x7E, 0x40, 0x40, 0x7C, 0x40, 0x40, 0x40, 0x00}, // F (70)
    {0x3C, 0x42, 0x40, 0x4E, 0x42, 0x42, 0x3C, 0x00}, // G (71)
    {0x42, 0x42, 0x42, 0x7E, 0x42, 0x42, 0x42, 0x00}, // H (72)
    {0x3E, 0x08, 0x08, 0x08, 0x08, 0x08, 0x3E, 0x00}, // I (73)
    {0x1E, 0x04, 0x04, 0x04, 0x04, 0x44, 0x38, 0x00}, // J (74)
    {0x42, 0x44, 0x48, 0x70, 0x48, 0x44, 0x42, 0x00}, // K (75)
    {0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x7E, 0x00}, // L (76)
    {0x42, 0x66, 0x5A, 0x5A, 0x42, 0x42, 0x42, 0x00}, // M (77)
    {0x42, 0x62, 0x52, 0x4A, 0x46, 0x42, 0x42, 0x00}, // N (78)
    {0x3C, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3C, 0x00}, // O (79)
    {0x7C, 0x42, 0x42, 0x7C, 0x40, 0x40, 0x40, 0x00}, // P (80)
    {0x3C, 0x42, 0x42, 0x42, 0x4A, 0x44, 0x3A, 0x00}, // Q (81)
    {0x7C, 0x42, 0x42, 0x7C, 0x48, 0x44, 0x42, 0x00}, // R (82)
    {0x3C, 0x42, 0x40, 0x3C, 0x02, 0x42, 0x3C, 0x00}, // S (83)
    {0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00}, // T (84)
    {0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3C, 0x00}, // U (85)
    {0x42, 0x42, 0x42, 0x42, 0x24, 0x24, 0x18, 0x00}, // V (86)
    {0x42, 0x42, 0x42, 0x5A, 0x5A, 0x66, 0x42, 0x00}, // W (87)
    {0x42, 0x24, 0x18, 0x18, 0x18, 0x24, 0x42, 0x00}, // X (88)
    {0x42, 0x42, 0x24, 0x18, 0x18, 0x18, 0x18, 0x00}, // Y (89)
    {0x7E, 0x04, 0x08, 0x10, 0x20, 0x40, 0x7E, 0x00}, // Z (90)
    {0x38, 0x20, 0x20, 0x20, 0x20, 0x20, 0x38, 0x00}, // [ (91)
    {0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x00, 0x00}, // \ (92)
    {0x38, 0x08, 0x08, 0x08, 0x08, 0x08, 0x38, 0x00}, // ] (93)
    {0x10, 0x28, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00}, // ^ (94)
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7E, 0x00}, // _ (95)
    {0x20, 0x10, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00}, // ` (96)
    {0x00, 0x00, 0x3C, 0x02, 0x3E, 0x42, 0x3E, 0x00}, // a (97)
    {0x40, 0x40, 0x5C, 0x62, 0x42, 0x62, 0x5C, 0x00}, // b (98)
    {0x00, 0x00, 0x3C, 0x42, 0x40, 0x42, 0x3C, 0x00}, // c (99)
    {0x02, 0x02, 0x3A, 0x46, 0x42, 0x46, 0x3A, 0x00}, // d (100)
    {0x00, 0x00, 0x3C, 0x42, 0x7E, 0x40, 0x3C, 0x00}, // e (101)
    {0x0C, 0x12, 0x10, 0x7C, 0x10, 0x10, 0x10, 0x00}, // f (102)
    {0x00, 0x00, 0x3A, 0x46, 0x46, 0x3A, 0x02, 0x3C}, // g (103)
    {0x40, 0x40, 0x5C, 0x62, 0x42, 0x42, 0x42, 0x00}, // h (104)
    {0x18, 0x00, 0x38, 0x18, 0x18, 0x18, 0x3C, 0x00}, // i (105)
    {0x06, 0x00, 0x06, 0x06, 0x06, 0x46, 0x3C, 0x00}, // j (106)
    {0x40, 0x40, 0x44, 0x48, 0x70, 0x48, 0x44, 0x00}, // k (107)
    {0x38, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3C, 0x00}, // l (108)
    {0x00, 0x00, 0x64, 0x5A, 0x52, 0x42, 0x42, 0x00}, // m (109)
    {0x00, 0x00, 0x5C, 0x62, 0x42, 0x42, 0x42, 0x00}, // n (110)
    {0x00, 0x00, 0x3C, 0x42, 0x42, 0x42, 0x3C, 0x00}, // o (111)
    {0x00, 0x00, 0x5C, 0x62, 0x62, 0x5C, 0x40, 0x40}, // p (112)
    {0x00, 0x00, 0x3A, 0x46, 0x46, 0x3A, 0x02, 0x02}, // q (113)
    {0x00, 0x00, 0x5C, 0x62, 0x40, 0x40, 0x40, 0x00}, // r (114)
    {0x00, 0x00, 0x3E, 0x40, 0x3C, 0x02, 0x7C, 0x00}, // s (115)
    {0x10, 0x10, 0x7C, 0x10, 0x10, 0x12, 0x0C, 0x00}, // t (116)
    {0x00, 0x00, 0x42, 0x42, 0x42, 0x46, 0x3A, 0x00}, // u (117)
    {0x00, 0x00, 0x42, 0x42, 0x42, 0x24, 0x18, 0x00}, // v (118)
    {0x00, 0x00, 0x42, 0x42, 0x52, 0x5A, 0x24, 0x00}, // w (119)
    {0x00, 0x00, 0x42, 0x24, 0x18, 0x24, 0x42, 0x00}, // x (120)
    {0x00, 0x00, 0x42, 0x42, 0x46, 0x3A, 0x02, 0x3C}, // y (121)
    {0x00, 0x00, 0x7E, 0x04, 0x18, 0x20, 0x7E, 0x00}, // z (122)
    {0x0C, 0x10, 0x10, 0x20, 0x10, 0x10, 0x0C, 0x00}, // { (123)
    {0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x00}, // | (124)
    {0x30, 0x08, 0x08, 0x04, 0x08, 0x08, 0x30, 0x00}, // } (125)
    {0x00, 0x32, 0x4C, 0x00, 0x00, 0x00, 0x00, 0x00}, // ~ (126)
};

static gui_t system_gui;

/* ═══════ Terminal Buffer ═══════ */
static char term_buf[TERM_BUF_ROWS][TERM_COLS + 1];
static int term_cur_row = 0, term_cur_col = 0;
static char term_input[TERM_INPUT_MAX + 1];
static int term_input_len = 0;

/* Scancode-to-ASCII (duplicated from io.c for GUI context) */
static const char gui_keymap[] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=','\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0,'a','s','d','f','g','h','j','k','l',';','\'','`',0,
    '\\','z','x','c','v','b','n','m',',','.','/',0,'*',0,' '
};
static const char gui_keymap_shift[] = {
    0,  27, '!','@','#','$','%','^','&','*','(',')','_','+','\b',
    '\t','Q','W','E','R','T','Y','U','I','O','P','{','}','\n',
    0,'A','S','D','F','G','H','J','K','L',':','"','~',0,
    '|','Z','X','C','V','B','N','M','<','>','?',0,'*',0,' '
};

void gui_term_putchar(char c) {
    if (c == '\n' || c == '\r') {
        term_cur_col = 0;
        term_cur_row++;
    } else if (c == '\t') {
        term_cur_col = (term_cur_col + 4) & ~3;
        if (term_cur_col >= TERM_COLS) { term_cur_col = 0; term_cur_row++; }
    } else if (c >= 32 && c <= 126) {
        if (term_cur_col >= TERM_COLS) { term_cur_col = 0; term_cur_row++; }
        if (term_cur_row >= TERM_BUF_ROWS) {
            for (int i = 0; i < TERM_BUF_ROWS - 1; i++)
                for (int j = 0; j <= TERM_COLS; j++) term_buf[i][j] = term_buf[i+1][j];
            for (int j = 0; j <= TERM_COLS; j++) term_buf[TERM_BUF_ROWS-1][j] = 0;
            term_cur_row = TERM_BUF_ROWS - 1;
        }
        term_buf[term_cur_row][term_cur_col++] = c;
    }
    if (term_cur_row >= TERM_BUF_ROWS) {
        for (int i = 0; i < TERM_BUF_ROWS - 1; i++)
            for (int j = 0; j <= TERM_COLS; j++) term_buf[i][j] = term_buf[i+1][j];
        for (int j = 0; j <= TERM_COLS; j++) term_buf[TERM_BUF_ROWS-1][j] = 0;
        term_cur_row = TERM_BUF_ROWS - 1;
    }
}

void gui_term_print(const char* str) {
    while (*str) {
        unsigned char c = (unsigned char)*str;
        if (c >= 0xE0 && c <= 0xEF && str[1] && str[2]) { str += 3; continue; }
        if (c >= 0xC0 && c <= 0xDF && str[1]) { str += 2; continue; }
        if (c >= 0xF0 && str[1] && str[2] && str[3]) { str += 4; continue; }
        gui_term_putchar(*str++);
    }
}

static void term_clear(void) {
    for (int i = 0; i < TERM_BUF_ROWS; i++)
        for (int j = 0; j <= TERM_COLS; j++) term_buf[i][j] = 0;
    term_cur_row = 0; term_cur_col = 0;
    term_input_len = 0;
}

/* ═══════ Core GUI ═══════ */

void gui_init(void) {
    system_gui.window_count = 0;
    system_gui.focused = -1;
    system_gui.desktop_color = 1;
    for (int i = 0; i < MAX_WINDOWS; i++) system_gui.z_order[i] = i;
    term_clear();
    io_set_gui_output(gui_term_print, gui_term_putchar);
    vga_set_mode_13h();
}

void gui_draw_desktop(void) {
    vga_clear(system_gui.desktop_color);
    vga_fill_rect(0, VGA_HEIGHT - 24, VGA_WIDTH, 24, 8);
    vga_fill_rect(0, VGA_HEIGHT - 24, VGA_WIDTH, 1, 15);
    gui_draw_string(10, VGA_HEIGHT - 16, "SkyNet OS", 15);
    /* Exit button */
    vga_fill_rect(VGA_WIDTH - 52, VGA_HEIGHT - 20, 44, 16, 4);
    gui_draw_string(VGA_WIDTH - 48, VGA_HEIGHT - 16, "Exit", 15);
}

void gui_create_window(const char* title, int x, int y, int w, int h,
                        color_t color, window_type_t type) {
    if (system_gui.window_count < MAX_WINDOWS) {
        int idx = system_gui.window_count;
        window_t* win = &system_gui.windows[idx];
        win->x = x; win->y = y; win->w = w; win->h = h;
        win->bg_color = color; win->active = true; win->type = type;
        strncpy(win->title, title, 31);
        system_gui.z_order[idx] = idx;
        system_gui.window_count++;
    }
}

void gui_draw_window(window_t* win) {
    if (!win->active) return;
    vga_fill_rect(win->x + 3, win->y + 3, win->w, win->h, 0);
    vga_fill_rect(win->x, win->y, win->w, win->h, 7);
    int focused = (&system_gui.windows[system_gui.focused] == win);
    color_t title_color = focused ? 9 : 8;
    vga_fill_rect(win->x + 1, win->y + 1, win->w - 2, 14, 1);
    vga_fill_rect(win->x + 1, win->y + 1, win->w - 2, 7, title_color);
    gui_draw_string(win->x + 8, win->y + 4, win->title, 15);
    vga_fill_rect(win->x + win->w - 14, win->y + 2, 12, 10, 4);
    gui_draw_char(win->x + win->w - 12, win->y + 4, 'X', 15);
    vga_fill_rect(win->x + 1, win->y + 15, win->w - 2, win->h - 16, win->bg_color);
}

void gui_render(void) {
    gui_draw_desktop();
    for (int i = 0; i < system_gui.window_count; i++)
        gui_draw_window(&system_gui.windows[i]);
}

void gui_draw_char(int x, int y, char c, color_t color) {
    if (x + 8 > VGA_WIDTH || y + 8 > VGA_HEIGHT) return;
    if (c < 32 || c > 126) return;
    const uint8_t* glyph = font8x8[c - 32];
    for (int i = 0; i < 8; i++) {
        uint8_t row = glyph[i];
        for (int j = 0; j < 8; j++)
            if (row & (0x80 >> j)) vga_put_pixel(x + j, y + i, color);
    }
}

void gui_draw_int(int x, int y, int n, color_t color) {
    if (n < 0) { gui_draw_char(x, y, '-', color); x += 8; n = -n; }
    if (n == 0) { gui_draw_char(x, y, '0', color); return; }
    char buf[12]; int len = 0;
    while (n > 0) { buf[len++] = '0' + (n % 10); n /= 10; }
    for (int i = len - 1; i >= 0; i--) { gui_draw_char(x, y, buf[i], color); x += 8; }
}

void gui_draw_string(int x, int y, const char* str, color_t color) {
    while (*str) { gui_draw_char(x, y, *str++, color); x += 8; }
}

/* ═══════ Window Content Renderers ═══════ */

void gui_draw_sysmon_content(window_t* win) {
    int cx = win->x + 8, cy = win->y + 24;
    gui_draw_string(cx, cy, "PID  PRIO  STATE  THRDS", 0); cy += 12;
    vga_fill_rect(cx, cy, win->w - 16, 1, 8); cy += 4;
    gui_draw_string(cx, cy, "0    0     RDY    1", 0); cy += 10;
    gui_draw_string(cx, cy, "1    0     RDY    1", 0); cy += 18;
    gui_draw_string(cx, cy, "CPU:  [##########      ] 62%", 2); cy += 10;
    gui_draw_string(cx, cy, "MEM:  [############    ] 75%", 4); cy += 10;
    gui_draw_string(cx, cy, "DISK: [####            ] 25%", 6); cy += 18;
    gui_draw_string(cx, cy, "Uptime: Tick-based scheduling", 8);
}

void gui_draw_about_content(window_t* win) {
    int cx = win->x + 8, cy = win->y + 24;
    gui_draw_string(cx, cy, "SkyNet OS v3.0", 0); cy += 12;
    vga_fill_rect(cx, cy, win->w - 16, 1, 8); cy += 6;
    gui_draw_string(cx, cy, "MLFQ Scheduler", 0); cy += 10;
    gui_draw_string(cx, cy, "Banker's Deadlock Avoidance", 0); cy += 10;
    gui_draw_string(cx, cy, "800x600 SVGA Desktop", 0); cy += 10;
    gui_draw_string(cx, cy, "IPC + Mutexes + Spinlocks", 0); cy += 14;
    gui_draw_string(cx, cy, "\"I'll be back.\" - 2029", 4);
}

void gui_draw_terminal_content(window_t* win) {
    int cx = win->x + 4, cy = win->y + 18;
    int max_rows = (win->h - 28) / 9;
    int start = term_cur_row - max_rows + 2;
    if (start < 0) start = 0;
    for (int r = start; r <= term_cur_row && r < TERM_BUF_ROWS; r++) {
        if (cy + 9 > win->y + win->h - 2) break;
        gui_draw_string(cx, cy, term_buf[r], 2);
        cy += 9;
    }
    /* Draw input line */
    if (cy + 9 <= win->y + win->h - 2) {
        gui_draw_string(cx, cy, "> ", 10);
        int ix = cx + 16;
        for (int i = 0; i < term_input_len; i++) {
            gui_draw_char(ix, cy, term_input[i], 15);
            ix += 8;
        }
        gui_draw_char(ix, cy, '_', 15); /* cursor */
    }
}

/* ═══════ Mouse Cursor ═══════ */

static const uint8_t cursor_data[12][8] = {
    {1,0,0,0,0,0,0,0},{1,1,0,0,0,0,0,0},{1,2,1,0,0,0,0,0},{1,2,2,1,0,0,0,0},
    {1,2,2,2,1,0,0,0},{1,2,2,2,2,1,0,0},{1,2,2,2,2,2,1,0},{1,2,2,2,1,1,1,0},
    {1,2,2,1,0,0,0,0},{1,2,1,0,0,0,0,0},{1,1,0,0,0,0,0,0},{1,0,0,0,0,0,0,0},
};
static int cursor_x, cursor_y;

void gui_draw_cursor(int x, int y) {
    for (int r = 0; r < 12; r++)
        for (int c = 0; c < 8; c++) {
            uint8_t v = cursor_data[r][c];
            if (v) vga_put_pixel(x + c, y + r, v == 1 ? 0 : 15);
        }
}

/* ═══════ Z-Order Helpers ═══════ */

static void raise_window(int idx) {
    int pos = -1;
    for (int i = 0; i < system_gui.window_count; i++)
        if (system_gui.z_order[i] == idx) { pos = i; break; }
    if (pos < 0) return;
    for (int i = pos; i < system_gui.window_count - 1; i++)
        system_gui.z_order[i] = system_gui.z_order[i + 1];
    system_gui.z_order[system_gui.window_count - 1] = idx;
    system_gui.focused = idx;
}

/* Find topmost window at (px, py), searching z_order back-to-front */
static int window_at(int px, int py) {
    for (int i = system_gui.window_count - 1; i >= 0; i--) {
        int idx = system_gui.z_order[i];
        window_t* w = &system_gui.windows[idx];
        if (!w->active) continue;
        if (px >= w->x && px < w->x + w->w && py >= w->y && py < w->y + w->h)
            return idx;
    }
    return -1;
}

static int in_title_bar(window_t* w, int px, int py) {
    return px >= w->x && px < w->x + w->w - 14 && py >= w->y && py < w->y + 15;
}

static int in_close_btn(window_t* w, int px, int py) {
    return px >= w->x + w->w - 14 && px < w->x + w->w - 2 && py >= w->y + 2 && py < w->y + 12;
}

/* ═══════ Full Scene Render ═══════ */

static void gui_full_render(void) {
    gui_draw_desktop();
    for (int i = 0; i < system_gui.window_count; i++) {
        int idx = system_gui.z_order[i];
        window_t* w = &system_gui.windows[idx];
        if (!w->active) continue;
        gui_draw_window(w);
        switch (w->type) {
            case WIN_SYSMON:   gui_draw_sysmon_content(w); break;
            case WIN_ABOUT:    gui_draw_about_content(w); break;
            case WIN_TERMINAL: gui_draw_terminal_content(w); break;
            default: break;
        }
    }
    gui_draw_cursor(cursor_x, cursor_y);
}

/* ═══════ Event Loop ═══════ */

void gui_event_loop(void) {
    mouse_init();
    cursor_x = VGA_WIDTH / 2;
    cursor_y = VGA_HEIGHT / 2;
    int shift_held = 0;
    int dragging = 0, drag_idx = -1, drag_ox = 0, drag_oy = 0;
    int prev_buttons = 0;

    /* Print welcome in terminal */
    gui_term_print("SkyNet Terminal v3.0\nType 'help' for commands.\n\n");
    gui_full_render();

    while (1) {
        uint8_t scancode = 0;
        int dx = 0, dy = 0, buttons = 0;
        int input = input_poll(&scancode, &dx, &dy, &buttons);

        if (input == INPUT_KEYBOARD) {
            /* Track shift state */
            if (scancode == 0x2A || scancode == 0x36) { shift_held = 1; continue; }
            if (scancode == 0xAA || scancode == 0xB6) { shift_held = 0; continue; }
            if (scancode & 0x80) continue; /* ignore other releases */

            if (scancode == 0x01) break; /* ESC exits */

            /* Route keyboard to terminal if focused */
            if (system_gui.focused >= 0 &&
                system_gui.windows[system_gui.focused].type == WIN_TERMINAL &&
                system_gui.windows[system_gui.focused].active) {
                if (scancode == 0x1C) { /* Enter */
                    term_input[term_input_len] = '\0';
                    gui_term_print("> ");
                    gui_term_print(term_input);
                    gui_term_putchar('\n');
                    if (strcmp(term_input, "exit") == 0) { break; }
                    else if (strcmp(term_input, "gui") == 0) {
                        gui_term_print("Already in GUI mode.\n");
                    } else if (strcmp(term_input, "clear") == 0) {
                        term_clear();
                    } else if (term_input_len > 0) {
                        gui_output_mode = true;
                        handle_command(term_input);
                        gui_output_mode = false;
                    }
                    term_input_len = 0;
                    gui_full_render();
                } else if (scancode == 0x0E) { /* Backspace */
                    if (term_input_len > 0) term_input_len--;
                    gui_full_render();
                } else if (scancode < sizeof(gui_keymap)) {
                    char ch = shift_held ? gui_keymap_shift[scancode] : gui_keymap[scancode];
                    if (ch >= 32 && ch <= 126 && term_input_len < TERM_INPUT_MAX) {
                        term_input[term_input_len++] = ch;
                        gui_full_render();
                    }
                }
            }
        } else if (input == INPUT_MOUSE) {
            cursor_x += dx; cursor_y += dy;
            if (cursor_x < 0) cursor_x = 0;
            if (cursor_x >= VGA_WIDTH) cursor_x = VGA_WIDTH - 1;
            if (cursor_y < 0) cursor_y = 0;
            if (cursor_y >= VGA_HEIGHT) cursor_y = VGA_HEIGHT - 1;

            int just_pressed = (buttons & 1) && !(prev_buttons & 1);
            int just_released = !(buttons & 1) && (prev_buttons & 1);
            prev_buttons = buttons;

            if (just_pressed) {
                /* Check Exit button */
                if (cursor_x >= VGA_WIDTH - 52 && cursor_x < VGA_WIDTH - 8 &&
                    cursor_y >= VGA_HEIGHT - 20 && cursor_y < VGA_HEIGHT - 4) {
                    break;
                }
                /* Check windows top-to-bottom in z-order */
                int hit = window_at(cursor_x, cursor_y);
                if (hit >= 0) {
                    window_t* w = &system_gui.windows[hit];
                    if (in_close_btn(w, cursor_x, cursor_y)) {
                        w->active = false;
                        if (system_gui.focused == hit) system_gui.focused = -1;
                    } else if (in_title_bar(w, cursor_x, cursor_y)) {
                        raise_window(hit);
                        dragging = 1; drag_idx = hit;
                        drag_ox = cursor_x - w->x; drag_oy = cursor_y - w->y;
                    } else {
                        raise_window(hit);
                    }
                }
            }
            if (just_released) { dragging = 0; drag_idx = -1; }

            if (dragging && drag_idx >= 0) {
                window_t* w = &system_gui.windows[drag_idx];
                w->x = cursor_x - drag_ox;
                w->y = cursor_y - drag_oy;
                if (w->x < 0) w->x = 0;
                if (w->y < 0) w->y = 0;
                if (w->x + w->w > VGA_WIDTH) w->x = VGA_WIDTH - w->w;
                if (w->y + w->h > VGA_HEIGHT - 24) w->y = VGA_HEIGHT - 24 - w->h;
            }
            gui_full_render();
        }
    }
    mouse_disable();
}
