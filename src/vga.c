#include "vga.h"
#include "string.h"
#include "io.h"

// BGA Ports and Indices
#define VBE_DISPI_IOPORT_INDEX 0x01CE
#define VBE_DISPI_IOPORT_DATA  0x01CF
#define VBE_DISPI_INDEX_ID     0x0
#define VBE_DISPI_INDEX_XRES   0x1
#define VBE_DISPI_INDEX_YRES   0x2
#define VBE_DISPI_INDEX_BPP    0x3
#define VBE_DISPI_INDEX_ENABLE 0x4
#define VBE_DISPI_INDEX_BANK   0x5
#define VBE_DISPI_DISABLED     0x00
#define VBE_DISPI_ENABLED      0x01
#define VBE_DISPI_LFB_ENABLED  0x40

// PCI Configuration Space
#define PCI_CONFIG_ADDR 0x0CF8
#define PCI_CONFIG_DATA 0x0CFC

// Bochs VGA PCI IDs
#define BOCHS_VGA_VENDOR 0x1234
#define BOCHS_VGA_DEVICE 0x1111

#ifdef __i386__

static inline void outl(uint16_t port, uint32_t val) {
    __asm__ volatile ("outl %0, %1" : : "a" (val), "Nd" (port));
}

static inline uint32_t inl(uint16_t port) {
    uint32_t ret;
    __asm__ volatile ("inl %1, %0" : "=a" (ret) : "Nd" (port));
    return ret;
}

static inline uint16_t inw(uint16_t port) {
    uint16_t ret;
    __asm__ volatile ("inw %1, %0" : "=a" (ret) : "Nd" (port));
    return ret;
}

#else

static inline void outl(uint16_t port, uint32_t val) { (void)port; (void)val; }
static inline uint32_t inl(uint16_t port) { (void)port; return 0; }
static inline uint16_t inw(uint16_t port) { (void)port; return 0; }

#endif

static void bga_write_register(uint16_t index, uint16_t data) {
    outw(VBE_DISPI_IOPORT_INDEX, index);
    outw(VBE_DISPI_IOPORT_DATA, data);
}


// Read a 32-bit value from PCI configuration space
static uint32_t pci_config_read(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address = (uint32_t)(
        ((uint32_t)1 << 31)       |   // Enable bit
        ((uint32_t)bus << 16)     |
        ((uint32_t)slot << 11)    |
        ((uint32_t)func << 8)     |
        (offset & 0xFC)
    );
    outl(PCI_CONFIG_ADDR, address);
    return inl(PCI_CONFIG_DATA);
}

// Scan PCI bus for Bochs VGA and return BAR0 (LFB address)
static uint32_t find_bochs_vga_lfb(void) {
    for (uint16_t bus = 0; bus < 256; bus++) {
        for (uint8_t slot = 0; slot < 32; slot++) {
            uint32_t id = pci_config_read(bus, slot, 0, 0);
            uint16_t vendor = id & 0xFFFF;
            uint16_t device = (id >> 16) & 0xFFFF;

            if (vendor == BOCHS_VGA_VENDOR && device == BOCHS_VGA_DEVICE) {
                // BAR0 is at offset 0x10
                uint32_t bar0 = pci_config_read(bus, slot, 0, 0x10);
                return bar0 & 0xFFFFFFF0; // Mask lower 4 bits (type/flags)
            }
        }
    }
    return 0xE0000000; // Fallback
}

// The framebuffer pointer - set dynamically
#ifdef __i386__
static uint8_t* vga_mem = (uint8_t*)0xA0000; // Will be updated at runtime
#else
static uint8_t mock_vga_mem[800 * 600];
static uint8_t* vga_mem = mock_vga_mem;
#endif

void vga_set_mode_13h() {
#ifdef __i386__
    // Find the real LFB address via PCI
    uint32_t lfb_addr = find_bochs_vga_lfb();
    vga_mem = (uint8_t*)lfb_addr;
#endif

    bga_write_register(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
    bga_write_register(VBE_DISPI_INDEX_XRES, VGA_WIDTH);
    bga_write_register(VBE_DISPI_INDEX_YRES, VGA_HEIGHT);
    bga_write_register(VBE_DISPI_INDEX_BPP, 8);
    bga_write_register(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED | VBE_DISPI_LFB_ENABLED);
}

void vga_set_mode_32bpp() {
#ifdef __i386__
    uint32_t lfb_addr = find_bochs_vga_lfb();
    vga_mem = (uint8_t*)lfb_addr;
#endif

    bga_write_register(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
    bga_write_register(VBE_DISPI_INDEX_XRES, VGA_WIDTH);
    bga_write_register(VBE_DISPI_INDEX_YRES, VGA_HEIGHT);
    bga_write_register(VBE_DISPI_INDEX_BPP, 32);
    bga_write_register(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED | VBE_DISPI_LFB_ENABLED);
}

void vga_set_text_mode() {
    bga_write_register(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
}

void vga_put_pixel(int x, int y, color_t color) {
    if (x >= 0 && x < VGA_WIDTH && y >= 0 && y < VGA_HEIGHT) {
        vga_mem[y * VGA_WIDTH + x] = color;
    }
}

void vga_fill_rect(int x, int y, int w, int h, color_t color) {
    for (int row = y; row < y + h; row++) {
        if (row < 0 || row >= VGA_HEIGHT) continue;
        for (int col = x; col < x + w; col++) {
            if (col >= 0 && col < VGA_WIDTH) {
                vga_mem[row * VGA_WIDTH + col] = color;
            }
        }
    }
}

void vga_clear(color_t color) {
    memset(vga_mem, color, VGA_WIDTH * VGA_HEIGHT);
}

uint8_t* vga_get_framebuffer() {
    return vga_mem;
}
