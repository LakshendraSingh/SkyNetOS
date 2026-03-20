## 0. Toolchain Installation

| Platform | Command |
|----------|---------|
| macOS | `brew install i686-elf-binutils i686-elf-gcc nasm qemu` |
| Ubuntu/Debian | `sudo apt install gcc-i686-linux-gnu nasm qemu-system-x86` |

## 1. QEMU (Recommended)
QEMU is the primary platform for SkyNetOS development.

### Quick Launch
```bash
make run
```
Or manually:
```bash
qemu-system-i386 -drive format=raw,file=bin/os.bin
```

### Useful QEMU Flags
| Flag | Effect |
|------|--------|
| `-m 128` | Set RAM to 128MB |
| `-monitor stdio` | QEMU monitor on terminal |
| `-d int` | Log interrupts (debugging) |
| `-no-reboot` | Halt instead of reboot on triple fault |

## 2. VirtualBox

### Configuration
1. **Create New VM**:
   - Name: `SkyNetOS`
   - Type: `Other`, Version: `Other/Unknown (32-bit)`
2. **Hardware**: 128MB RAM, 1 CPU core
3. **Storage**:
   - Remove the default disk
   - Add an **IDE Controller** with `bin/os.bin` as a raw disk
   - You may need to rename to `os.img` for VirtualBox compatibility
4. **System**: Ensure **Legacy/CSM boot** is enabled

## 3. Resource Requirements

| Resource | Minimum | Recommended |
|----------|---------|-------------|
| CPU | i386 compatible | — |
| RAM | 8MB | 128MB |
| Disk | 128KB (image size) | — |
| Display | VGA compatible | BGA-capable (QEMU default) |
