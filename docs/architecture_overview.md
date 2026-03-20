# Architecture Overview

## Introduction
SkyNetOS is a baremetal x86 operating system written from scratch in C and NASM assembly. It boots via a custom MBR bootloader, transitions to 32-bit protected mode, and runs a monolithic kernel with integrated shell and GUI.

## System Stack
```
┌──────────────────────────────────────┐
│         GUI (800×600 SVGA)           │
│    Window Manager + ASCII Font       │
├──────────────────────────────────────┤
│         Interactive Shell            │
│  (12+ commands, background tasks)    │
├──────────┬───────────┬───────────────┤
│ Security │ Scheduler │  File System  │
│ (SHA-3)  │  (MLFQ)   │  (In-Memory) │
├──────────┴───────────┴───────────────┤
│    Memory Manager (Pages + Heap)     │
├──────────────────────────────────────┤
│  I/O (VGA Text + PS/2 Keyboard)     │
│  Shift support, scrolling, masking   │
├──────────────────────────────────────┤
│  VGA Driver (BGA + PCI LFB detect)  │
├──────────────────────────────────────┤
│  Bootloader (MBR → Protected Mode)  │
└──────────────────────────────────────┘
```

## Components

| Component | File(s) | Description |
|-----------|---------|-------------|
| Bootloader | `bootloader.asm` | 512-byte MBR, BIOS disk read, GDT, real→protected mode |
| Kernel | `kernel.c`, `kernel.asm` | Init sequence, login with SHA-3 auth, error handler |
| I/O | `io.c`, `io.h` | VGA text output, PS/2 keyboard with Shift, scrolling |
| Shell | `shell.c` | 12+ commands including `ls`, `cat`, `touch`, `mkdir`, `gui` |
| VGA/GUI | `vga.c`, `gui.c` | BGA 800×600 SVGA, PCI BAR0 detection, window manager |
| Scheduler | `scheduler.c` | MLFQ (3 queues) + Banker's Algorithm |
| Memory | `memory_management.c` | 1024×4KB page allocator + 1MB heap |
| File System | `file_system.c` | In-memory VFS with files and directories |
| Security | `security.c`, `sha3.c` | SHA-3 256 password hashing |

## Boot Sequence
1. BIOS loads Sector 0 (bootloader) to `0x7C00`
2. Bootloader reads 127 sectors via `int 0x13` to `0x10000`
3. GDT is loaded, CPU switches to 32-bit protected mode
4. `kernel_main()` initializes memory, file system, scheduler
5. Login prompt authenticates user via SHA-3
6. Shell starts with `SkyNet >` prompt