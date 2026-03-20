#!/bin/bash
# run_qemu.sh - Launch SkynetOS in QEMU

BIN_PATH="bin/os.bin"

if [ ! -f "$BIN_PATH" ]; then
    echo "Error: $BIN_PATH not found. Run 'make' first."
    exit 1
fi

echo "Launching SkynetOS in QEMU..."
qemu-system-i386 \
    -drive format=raw,file=$BIN_PATH \
    -m 128 \
    -serial stdio \
    -no-reboot
