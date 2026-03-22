# SkyNetOS Changelog

## v3.0 — Advanced OS Systems

### Concurrency & Synchronization
- **Atomic Spinlocks**: Implemented cross-architecture (`__atomic_test_and_set`) locks explicitly protecting physical page mappings and simulated disk environments without starvation.
- **Blocking Mutexes**: Engineered `mutex_t` deeply intertwined with the `scheduler.c` logic, allowing tasks to gracefully un-schedule themselves from the active CPU loop into `TASK_BLOCKED` barriers mitigating excessive polling natively.

### Process Management & IPC
- **IPC Mailboxes**: Bounded asynchronous queues enabling secure cross-process communications locally protected by `mutex_t` limits natively (up to 5 static messages). Exposed by the shell as `ipc-send` & `ipc-recv`.
- **Multithreading logic**: Appended explicit thread-tracking scopes to the core OS TCB limits (`thread-add`).
- **Asynchronous Ticking**: Attached MLFQ quantum updates systematically into `io.c`'s idle loop, granting background simulations reliable progress outside explicit `schedule()` commands.

### Memory & Storage 
- **Process Isolation**: Extended the `allocate_process_memory()` pipeline statically capturing distinct `owner_pid` identifiers natively denying non-kernel remote-process deallocations seamlessly.
- **Disk Scheduling**: Implemented `disk_scheduler.c` iteratively assessing active arrays sorting natively executing tracking against Shortest Seek Time First (SSTF) algorithms optimally minimizing mock contiguous seeks recursively.

### Shell — 5 New Commands
- `edit <file>` — Formally integrated an interactive text-editing loop supporting dynamic buffer writes!
- `ipc-send <pid> <msg>` — Transmit process messages.
- `ipc-recv <pid>` — Print explicit tasks' oldest mail messages cleanly.
- `thread-add <pid>` — Modify TCB bounds explicitly manually testing structures tracking bounds logically.
- `disk-test` — Kick off SSTF native sorting iterations testing structural outputs transparently.

### Bug Fixes
- **Scheduler Spam**: Removed intensive "No tasks ready to run" console printing sequences triggered natively by asynchronous `system_tick` polling loops evaluating idle queues on boot.

---

## v2.0 — Baremetal OS Complete

### GUI — 800×600 SVGA Desktop
- Upgraded display from 320×200 to **800×600** using BGA + Linear Frame Buffer.
- **PCI BAR0 Detection**: VGA driver scans PCI bus to find the correct LFB address at runtime.
- **Full ASCII Font**: Complete printable ASCII (chars 32–126) with 8×8 bitmap glyphs.
- **Window Manager**: Overlapping windows with title bars, close buttons, and drop shadows.
- Desktop taskbar with "SkyNet OS" branding.

### Shell — 7 New Commands
- `clear` — Clear the screen
- `ls` — List files and directories with sizes and type indicators
- `cat <file>` — Display file contents
- `touch <file>` — Create an empty file
- `mkdir <dir>` — Create a directory
- `rm <file>` — Delete a file
- `whoami` — Display current user

### I/O — Keyboard & Display
- **Shift Key Support**: Full shifted keymap for uppercase letters and symbols (`_`, `!`, `@`, `#`, etc.).
- **Screen Scrolling**: Text scrolls up when the screen is full instead of wrapping.
- **Password Masking**: Login displays `*` instead of plaintext.
- **Tab Support**: Tab key advances to next 8-column boundary.

### Code Quality
- Fixed memory leak in `write_file()` (old data wasn't freed before reallocation).
- Removed stray `src/a.out` binary.
- Removed unused `bga_read_register()` function (compiler warning).
- Doubled disk image capacity from 64KB to 128KB.

---

## v1.0 — Initial Release

### Bootloader
- Custom 512-byte MBR bootloader with BIOS `int 0x13` disk read.
- GDT setup and real → protected mode transition.
- Loads kernel to `0x10000`.

### Kernel
- 32-bit protected mode C kernel.
- SHA-3 256 password authentication.
- Centralized error handler.

### Scheduler
- Multi-Level Feedback Queue (MLFQ) with 3 priority levels.
- Banker's Algorithm for deadlock avoidance.
- Priority boosting to prevent starvation.

### Memory
- 1024 × 4KB page allocator with Next-Fit strategy.
- 1MB heap with malloc/free, block splitting, and coalescing.

### File System
- In-memory VFS with files and directories.
- Create, read, write, delete operations.

### Shell
- Interactive command-line with `help`, `ps`, `kill`, `sim-load`, `exit`.
- Background task execution with `&` operator.

### Testing & CI
- Unit test suite in `tests/` for security, scheduler, memory, file system, and I/O.
- GitHub Actions CI pipeline.
