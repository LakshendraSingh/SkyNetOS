#!/bin/bash

# SkynetOS Build & Environment Setup Script
echo "--- SkynetOS Build System ---"

# Detect cross-compiler
if command -v i686-elf-gcc &> /dev/null; then
    export CROSS_COMPILE=i686-elf-
    echo "Using i686-elf-gcc toolchain (Homebrew/Standard)"
elif command -v i386-elf-gcc &> /dev/null; then
    export CROSS_COMPILE=i386-elf-
    echo "Using i386-elf-gcc toolchain"
else
    # Check common manual install path
    PREFIX="$HOME/opt/cross/bin"
    if [ -d "$PREFIX" ] && [ -f "$PREFIX/i386-elf-gcc" ]; then
        export PATH="$PREFIX:$PATH"
        export CROSS_COMPILE=i386-elf-
        echo "Using toolchain from $PREFIX"
    else
        echo "ERROR: Cross-compiler (i386-elf-gcc or i686-elf-gcc) not found."
        echo "Please install via Homebrew: brew install i686-elf-binutils i686-elf-gcc"
        exit 1
    fi
fi

# Run build
make all
if [ $? -eq 0 ]; then
    echo "Build successful: bin/os.bin created."
    echo "Run 'make run' to test in QEMU."
else
    echo "Build failed."
    exit 1
fi