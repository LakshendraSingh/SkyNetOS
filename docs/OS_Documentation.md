# SkyNetOS Technical Documentation

## 1. Architecture
SkyNetOS is a monolithic x86 operating system with a custom bootloader, 32-bit protected mode kernel, interactive shell, and SVGA graphical interface. See [architecture_overview.md](architecture_overview.md) for the full system diagram.

## 2. Memory Management
Implemented in `memory_management.c`:
- **Page Allocator**: 1024 × 4KB pages using **Next-Fit** strategy for fast free-page discovery.
- **Heap Allocator**: 1MB slab with block-based `malloc`/`free`, block splitting, and coalescing on `free()`.

## 3. CPU Scheduling
Implemented in `scheduler.c`:

### MLFQ (Multi-Level Feedback Queue)
- **3 Queues** (Q0–Q2) with time quantums of 4, 8, and 16 units.
- **Dynamic Demotion**: Tasks that exhaust their quantum are moved to lower-priority queues.
- **Priority Boosting**: Periodic reset of all tasks to Q0 to prevent starvation.
- **Round Robin**: Within each queue level.

### Banker's Algorithm (Deadlock Avoidance)
- Simulates resource allocation before granting requests.
- Maintains `Available`, `Max`, `Allocation`, and `Need` matrices.
- Rolls back unsafe allocations to prevent deadlocks.

## 4. Security
- **SHA-3 256**: Keccak-p[1600, 24] based password hashing in `sha3.c`.
- **Password Masking**: Login input displays `*` instead of plaintext characters.
- **Input Sanitization**: All buffers are bounded to prevent overflow.

## 5. I/O Subsystem
Implemented in `io.c`:
- **VGA Text Mode**: Direct memory-mapped output at `0xB8000` (80×25).
- **Screen Scrolling**: Text scrolls up automatically when the screen is full.
- **PS/2 Keyboard**: Scan-code-to-ASCII translation with full Shift key support (uppercase, symbols like `_`, `!`, `@`).
- **Tab Support**: Advances cursor to next 8-column boundary.

## 6. File System
Implemented in `file_system.c`:
- **In-Memory VFS**: Flat directory structure with `file_t` and `directory_t` types.
- **Operations**: `create_file`, `write_file`, `read_file`, `delete_file`, `create_directory`, `delete_directory`.
- **Shell Integration**: Exposed via `ls`, `cat`, `touch`, `mkdir`, `rm` commands.

## 7. Graphics
### VGA Driver (`vga.c`)
- **BGA Interface**: Bochs Graphics Adapter for SVGA mode switching.
- **PCI Detection**: Scans PCI bus for Bochs VGA device (vendor `0x1234`, device `0x1111`) to find the LFB address at runtime via BAR0.
- **Resolution**: 800×600 with 8-bit color depth.

### GUI (`gui.c`)
- **Window Manager**: Supports multiple overlapping windows with title bars, close buttons, and drop shadows.
- **Font Engine**: Complete printable ASCII font (chars 32–126) using 8×8 bitmap glyphs.
- **Desktop**: Taskbar at screen bottom, blue desktop background.

## 8. Bootloader
Implemented in `bootloader.asm`:
- 512-byte MBR with `0xAA55` signature.
- Reads 127 sectors via BIOS `int 0x13` to load the kernel at `0x10000`.
- Sets up GDT and transitions CPU from 16-bit real mode to 32-bit protected mode.
- Jumps to `kernel_main()` entry point.
