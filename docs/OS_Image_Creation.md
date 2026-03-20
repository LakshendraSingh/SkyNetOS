# OS Image Creation Guide

This document describes how the SkyNetOS bootable image (`os.bin`) is assembled.

## Image Structure
The bootable image is a **128KB** raw disk image (256 sectors × 512 bytes):

| Sector | Contents |
|--------|----------|
| 0 | Bootloader (512 bytes, ends with `0xAA55`) |
| 1–127 | Kernel binary (`kernel.bin`, ~28KB) |
| 128–255 | Reserved (zero-padded) |

## Creation Process
The `Makefile` assembles the image using `dd`:

```bash
# 1. Create blank 128KB image
dd if=/dev/zero of=bin/os.bin bs=512 count=256

# 2. Write bootloader to sector 0
dd if=bin/bootloader.bin of=bin/os.bin conv=notrunc

# 3. Write kernel starting at sector 1
dd if=bin/kernel.bin of=bin/os.bin seek=1 conv=notrunc
```

## Build Pipeline
```
bootloader.asm  →  NASM  →  bootloader.bin (512 bytes)

*.c sources  →  i686-elf-gcc  →  *.o objects
kernel.asm   →  NASM          →  kernel.asm.o
                    ↓
              i686-elf-ld  →  kernel.elf
                    ↓
              objcopy      →  kernel.bin (~28KB)

bootloader.bin + kernel.bin  →  dd  →  os.bin (128KB)
```

## Tools
| Tool | Purpose |
|------|---------|
| `nasm` | Assembles bootloader and kernel entry |
| `i686-elf-gcc` | Compiles C sources to i386 objects |
| `i686-elf-ld` | Links all objects into kernel ELF |
| `objcopy` | Converts ELF to flat binary |
| `dd` | Assembles the final disk image |
