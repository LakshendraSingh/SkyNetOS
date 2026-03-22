# SkyNetOS User Guide

Welcome to SkyNetOS! This guide covers building, running, and using the operating system.

SkyNetOS User Guide

Quick Reference:
Login: admin / skynet
Build: ./build.sh && make run

Shell Commands:
help, clear, whoami, ls, cat, touch, mkdir, rm
ps, kill, sim-load, gui, exit

GUI: Type 'gui' for 800x600 SVGA desktop
Exit: Press any key to return to shell

## 1. Getting Started

### Prerequisites

| Tool           | macOS (Homebrew)            | Linux (apt)                      |
| -------------- | --------------------------- | -------------------------------- |
| Cross-compiler | `brew install i686-elf-gcc` | `apt install gcc-i686-linux-gnu` |
| Assembler      | `brew install nasm`         | `apt install nasm`               |
| Emulator       | `brew install qemu`         | `apt install qemu-system-x86`    |

### Build & Run

```bash
git clone https://github.com/LakshendraSingh/SkyNetOS.git
cd SkyNetOS
./build.sh       # Compile everything
make run          # Launch in QEMU
```

### Login

```
Username: admin
Password: skynet
```

> Password input is masked with `*` characters for security.

## 2. Shell Commands

| Command        | Description                                  |
| -------------- | -------------------------------------------- |
| `help`         | Display all available commands               |
| `clear`        | Clear the screen                             |
| `whoami`       | Display current user                         |
| `ls`           | List files and directories with sizes        |
| `cat <file>`   | Display file contents                        |
| `touch <file>` | Create an empty file                         |
| `mkdir <dir>`  | Create a directory                           |
| `rm <file>`    | Delete a file                                |
| `ps`           | List active processes                        |
| `kill <pid>`   | Terminate a process                          |
| `sim-load`     | Simulate a workload (add `&` for background) |
| `edit <file>`  | Interactive text editor to modify files      |
| `ipc-send`     | Send an IPC message: `ipc-send <pid> <msg>`  |
| `ipc-recv`     | Receive an IPC message: `ipc-recv <pid>`     |
| `thread-add`   | Add a thread to a PID: `thread-add <pid>`    |
| `disk-test`    | Run the SSTF disk scheduler test             |
| `gui`          | Enter graphical desktop mode                 |
| `exit`         | Shutdown the system                          |

### Keyboard Support

- Full US QWERTY layout with **Shift key** support
- Uppercase letters: `Shift + letter`
- Symbols: `_`, `!`, `@`, `#`, `$`, `%`, `^`, `&`, `*`, `(`, `)`, etc.
- Backspace and Tab are supported

## 3. Graphical User Interface

Type `gui` in the shell to enter the desktop environment.

- **Resolution**: 800×600 SVGA (256 colors)
- **Features**: Desktop taskbar, overlapping windows with title bars and close buttons
- **Font**: Full printable ASCII (all letters, digits, and punctuation)
- **Exit**: Press any key to return to the text shell

## 4. Process Management

SkyNetOS uses a **Multi-Level Feedback Queue (MLFQ)** scheduler:

- **Background tasks**: Append `&` to run a command in the background. Their quantum is seamlessly advanced during input wait cycles using `system_tick()`.
- **Monitor**: Use `ps` to list running processes, active threads, and pending IPC mailbox size.
- **Terminate**: Use `kill <pid>` to stop a process
- **Safety**: The Banker's Algorithm prevents deadlocks during resource allocation

## 5. Error Codes

| Code | Meaning                          |
| ---- | -------------------------------- |
| 2    | Out of memory                    |
| 3    | File or directory not found      |
| 10   | File or directory already exists |

## 6. FAQ

**Q: Screen fills up and text disappears.**
A: The screen now auto-scrolls. If you need a fresh start, type `clear`.

**Q: `make` fails with compiler not found.**
A: Ensure your cross-compiler is installed and in your PATH.

**Q: Password doesn't show when typing.**
A: That's by design — input is masked with `*` for security.
