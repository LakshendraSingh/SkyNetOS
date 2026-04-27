# SkyNetOS

A baremetal x86 operating system built from scratch — bootloader, kernel, shell, GUI, and all.

![x86](https://img.shields.io/badge/arch-x86-blue) ![NASM](https://img.shields.io/badge/asm-NASM-orange) ![C](https://img.shields.io/badge/lang-C99-green) ![License](https://img.shields.io/badge/license-MIT-lightgrey)

## Features

| Component | Description |
|-----------|-------------|
| **Bootloader** | Custom 512-byte MBR bootloader with real→protected mode transition |
| **Kernel** | 32-bit protected mode C kernel with modular architecture |
| **Shell** | Interactive command-line interface with 15+ commands |
| **GUI** | 800×600 SVGA desktop with window manager (BGA + LFB) |
| **Networking**| TCP/IP stack with DNS resolution (RTL8139 driver) (**In Development**)|
| **Browser** | Simple text-based HTTP browser (**In Development**) |
| **Scheduler** | MLFQ with background simulated ticks and basic IPC messaging |
| **Multithreading**| Task thread counters via TCB simulation |
| **Memory** | Page-based allocator with basic Process Isolation via PID tagging |
| **File System** | In-memory VFS with files, directories, and interactive `edit` capability |
| **Disk** | Simulated Mock Disk Scheduler utilizing Shortest Seek Time First (SSTF) |
| **Security** | SHA-3 256 password authentication with masked input |
| **Keyboard** | PS/2 driver with full US QWERTY + Shift support + asynchronous background context ticks |

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
  edit <f>   - Simple text editor (:wq to save)
  ps         - List active processes
  kill <pid> - Terminate a process
  sim-load   - Run simulated workload (& for bg)
  ipc-send   - Send IPC msg: ipc-send <pid> <msg>
  ipc-recv   - Read IPC msg from process mailbox
  disk-test  - Run SSTF disk scheduler test
  gui        - Start GUI mode
  doom       - Play DOOM (Ported)
  browser <u>- HTTP browser (supports IP or Domain)
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
│      Networking (TCP/IP + DNS)       │
├──────────────────────────────────────┤
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
├── net.c / net.h       # TCP/IP stack + DNS resolution
├── browser.c / .h      # Simple HTTP browser (In Dev)
├── rtl8139.c / .h      # Realtek 8139 Network driver
├── pci.c / pci.h       # PCI bus scanning
├── scheduler.c / .h    # MLFQ task scheduler
├── memory_management.c # Page + heap allocator
├── file_system.c / .h  # In-memory file system
├── disk_scheduler.c/.h # SSTF disk scheduling logic
├── security.c / .h     # SHA-3 authentication
├── sha3.c / sha3.h     # SHA-3 256 implementation
├── string.c / .h       # Freestanding libc subset
└── linkerscript.ld     # Kernel memory layout
```

## Technical Details

- **Boot**: BIOS `int 0x13` loads 127 sectors → GDT → protected mode → `kernel_main()`
- **Display**: PCI bus scan detects Bochs VGA BAR0 for LFB address at runtime
- **Keyboard**: Interrupt-driven PS/2 with Shift state tracking and scan-code translation
- **Memory**: 1024 × 4KB pages + 1MB slab heap; basic isolation tracking via PID tagging.
- **Deadlock Prevention**: Banker's Algorithm for resource allocation safety checks.
- **Concurrency Defenses**: Resolves classic critical section constraints intrinsically distributing explicit `spinlock_t` and queuing `mutex_t` blocks securely across all asynchronous mutations tracking safe context states directly. 
- **IPC & Background Processing**: Mailbox queues supporting up to 5 unread messages strictly isolated per task natively locked by Mutex bounds; system ticks inject asynchronous background MLFQ updates dynamically.
- **Disk Scheduling**: FCFS/SSTF algorithms exposed virtually to sequence IO natively monitored strictly by independent Atomic Spinlocks minimizing algorithmic faults.

## License

MIT
