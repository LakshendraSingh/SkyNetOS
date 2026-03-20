# SkyNetOS

A baremetal x86 operating system built from scratch — bootloader, kernel, shell, GUI, and all.

![x86](https://img.shields.io/badge/arch-x86-blue) ![NASM](https://img.shields.io/badge/asm-NASM-orange) ![C](https://img.shields.io/badge/lang-C99-green) ![License](https://img.shields.io/badge/license-MIT-lightgrey)

## Features

| Component | Description |
|-----------|-------------|
| **Bootloader** | Custom 512-byte MBR bootloader with real→protected mode transition |
| **Kernel** | 32-bit protected mode C kernel with modular architecture |
| **Shell** | Interactive command-line interface with 12+ commands |
| **GUI** | 800×600 SVGA desktop with window manager (BGA + LFB) |
| **Scheduler** | Multi-Level Feedback Queue (MLFQ) with round-robin |
| **Memory** | Page-based allocator + heap manager with coalescing |
| **File System** | In-memory VFS with files and directories |
| **Security** | SHA-3 256 password authentication with masked input |
| **Keyboard** | PS/2 driver with full US QWERTY + Shift support |

## Quick Start

### Prerequisites
- **macOS/Linux** with Homebrew
- QEMU (`brew install qemu`)
- i686-elf cross-compiler (`brew install i686-elf-gcc`)
- NASM (`brew install nasm`)

### Build & Run
```bash
git clone https://github.com/LakshendraSingh/SkyNetOS.git
cd SkyNetOS
./build.sh
make run
```

### Default Login
```
Username: admin
Password: skynet
```

## Shell Commands

```
SkyNet > help
  help       - Display this help message
  clear      - Clear the screen
  whoami     - Display current user
  ls         - List files and directories
  cat <file> - Display file contents
  touch <f>  - Create an empty file
  mkdir <d>  - Create a directory
  rm <file>  - Delete a file
  ps         - List active processes
  kill <pid> - Terminate a process
  sim-load   - Run simulated workload (& for bg)
  gui        - Start GUI mode
  exit       - Shutdown the system
```

## Architecture

```
┌──────────────────────────────────────┐
│            GUI (800×600)             │
│     Window Manager + Font Engine     │
├──────────────────────────────────────┤
│          Interactive Shell           │
├──────────┬───────────┬───────────────┤
│ Security │ Scheduler │  File System  │
│  (SHA-3) │  (MLFQ)   │   (In-Mem)   │
├──────────┴───────────┴───────────────┤
│     Memory Manager (Pages + Heap)    │
├──────────────────────────────────────┤
│   I/O Layer (VGA + PS/2 Keyboard)    │
├──────────────────────────────────────┤
│  VGA Driver (BGA + PCI LFB detect)   │
├──────────────────────────────────────┤
│    Bootloader (MBR → Protected Mode) │
└──────────────────────────────────────┘
```

## Project Structure

```
src/
├── bootloader.asm      # 16-bit MBR bootloader
├── kernel.asm          # 32-bit entry point
├── kernel.c            # Kernel main + login
├── io.c / io.h         # Console I/O + keyboard driver
├── vga.c / vga.h       # VGA/BGA graphics driver
├── gui.c / gui.h       # Window manager + font engine
├── shell.c / shell.h   # Command-line shell
├── scheduler.c / .h    # MLFQ task scheduler
├── memory_management.c # Page + heap allocator
├── file_system.c / .h  # In-memory file system
├── security.c / .h     # SHA-3 authentication
├── sha3.c / sha3.h     # SHA-3 256 implementation
├── string.c / .h       # Freestanding libc subset
├── error_codes.h       # System error definitions
└── linkerscript.ld     # Kernel memory layout
```

## Technical Details

- **Boot**: BIOS `int 0x13` loads 127 sectors → GDT → protected mode → `kernel_main()`
- **Display**: PCI bus scan detects Bochs VGA BAR0 for LFB address at runtime
- **Keyboard**: Interrupt-driven PS/2 with Shift state tracking and scan-code translation
- **Memory**: 1024 × 4KB pages + 1MB slab heap with block splitting and coalescing
- **Deadlock Prevention**: Banker's Algorithm for resource allocation safety checks

## License

MIT
