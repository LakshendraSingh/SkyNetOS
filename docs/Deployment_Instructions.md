# Deployment Instructions

Step-by-step instructions for deploying SkyNetOS to virtual and physical hardware.

## 1. Virtual Deployment (Recommended)

### Quick Start

```bash
./build.sh       # Compile
make run          # Launch in QEMU
```

### Manual QEMU Launch

```bash
qemu-system-i386 -drive format=raw,file=bin/os.bin
```

See [Virtualization_Setup.md](Virtualization_Setup.md) for VirtualBox instructions and advanced QEMU flags.

## 2. Physical Deployment

### Creating a Bootable USB

> [!WARNING]
> This will **erase all data** on the target drive. Double-check the device path.

#### macOS

```bash
diskutil list                    # Find your USB device
sudo dd if=bin/os.bin of=/dev/diskN bs=512 conv=notrunc
```

#### Linux

```bash
lsblk                           # Find your USB device
sudo dd if=bin/os.bin of=/dev/sdX bs=512 conv=notrunc status=progress
```

#### Windows

Use **Rufus** or **balenaEtcher**:

1. Select `bin/os.bin` as the source image
2. Select your USB drive
3. Choose "DD Image" / "Raw Image" mode
4. Click Start

### Booting

1. Restart and enter BIOS/UEFI setup
2. Enable **Legacy/CSM boot** (SkyNetOS uses MBR, not UEFI)
3. Set the USB drive as first boot device
4. Save and reboot

## Troubleshooting

| Symptom                    | Fix                                               |
| -------------------------- | ------------------------------------------------- |
| "No bootable device"       | Verify `0xAA55` boot signature in sector 0        |
| Kernel panic / reboot loop | Check kernel is at sector 1 in the image          |
| Memory errors              | Ensure at least 8MB RAM available                 |
| No display in GUI mode     | Check for BGA-compatible VGA (QEMU default works) |
