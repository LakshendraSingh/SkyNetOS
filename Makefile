# Detect cross-compiler
ifeq ($(shell command -v i386-elf-gcc >/dev/null 2>&1 && echo found),found)
    CROSS_COMPILE ?= i386-elf-
else ifeq ($(shell command -v i686-elf-gcc >/dev/null 2>&1 && echo found),found)
    CROSS_COMPILE ?= i686-elf-
else
    CROSS_COMPILE ?= i386-elf-
endif

CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy
AS = nasm

FILES = ./build/kernel.asm.o ./build/kernel.o ./build/io.o ./build/shell.o ./build/security.o ./build/memory_management.o ./build/file_system.o ./build/string.o ./build/sha3.o ./build/scheduler.o ./build/vga.o ./build/gui.o ./build/disk_scheduler.o ./build/sync.o
FLAGS = -g -ffreestanding -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc -std=gnu99

all: 
	$(AS) -f bin ./src/bootloader.asm -o ./bin/bootloader.bin
	$(AS) -f elf -g ./src/kernel.asm -o ./build/kernel.asm.o
	$(CC) -I ./src $(FLAGS) -c ./src/kernel.c -o ./build/kernel.o
	$(CC) -I ./src $(FLAGS) -c ./src/io.c -o ./build/io.o
	$(CC) -I ./src $(FLAGS) -c ./src/shell.c -o ./build/shell.o
	$(CC) -I ./src $(FLAGS) -c ./src/security.c -o ./build/security.o
	$(CC) -I ./src $(FLAGS) -c ./src/memory_management.c -o ./build/memory_management.o
	$(CC) -I ./src $(FLAGS) -c ./src/file_system.c -o ./build/file_system.o
	$(CC) -I ./src $(FLAGS) -c ./src/string.c -o ./build/string.o
	$(CC) -I ./src $(FLAGS) -c ./src/sha3.c -o ./build/sha3.o
	$(CC) -I ./src $(FLAGS) -c ./src/scheduler.c -o ./build/scheduler.o
	$(CC) -I ./src $(FLAGS) -c ./src/vga.c -o ./build/vga.o
	$(CC) -I ./src $(FLAGS) -c ./src/gui.c -o ./build/gui.o
	$(CC) -I ./src $(FLAGS) -c ./src/disk_scheduler.c -o ./build/disk_scheduler.o
	$(CC) -I ./src $(FLAGS) -c ./src/sync.c -o ./build/sync.o
	$(LD) -g -relocatable $(FILES) -o ./build/completeKernel.o
	$(LD) -g -T ./src/linkerscript.ld -o ./build/kernel.elf ./build/completeKernel.o
	$(OBJCOPY) -O binary ./build/kernel.elf ./bin/kernel.bin
	dd if=/dev/zero of=./bin/os.bin bs=512 count=256
	dd if=./bin/bootloader.bin of=./bin/os.bin conv=notrunc
	dd if=./bin/kernel.bin of=./bin/os.bin seek=1 conv=notrunc

run: all
	qemu-system-i386 -display cocoa,zoom-to-fit=on -drive format=raw,file=./bin/os.bin

test:
	gcc -O0 -I./src src/security.c src/sha3.c tests/test_security.c -o test_security && ./test_security
	gcc -O0 -I./src src/sync.c src/scheduler.c src/io.c tests/test_scheduler.c -o test_scheduler && ./test_scheduler
	gcc -O0 -I./src src/sync.c src/scheduler.c src/io.c src/file_system.c src/memory_management.c tests/test_file_system.c -o test_file_system && ./test_file_system
	gcc -O0 -I./src src/sync.c src/scheduler.c src/io.c src/memory_management.c tests/test_memory_management.c -o test_memory_management && ./test_memory_management
	gcc -O0 -I./src src/sync.c src/scheduler.c src/io.c tests/test_io.c -o test_io && ./test_io
	gcc -O0 -I./src src/sync.c src/scheduler.c src/io.c src/file_system.c src/memory_management.c tests/test_directories.c -o test_directories && ./test_directories
	gcc -O0 -I./src src/sync.c src/scheduler.c src/io.c src/disk_scheduler.c tests/test_disk_scheduler.c -o test_disk_scheduler && ./test_disk_scheduler
	gcc -O0 -I./src src/sync.c src/scheduler.c src/io.c tests/test_sync.c -o test_sync && ./test_sync
	rm -f test_security test_scheduler test_file_system test_memory_management test_io test_directories test_disk_scheduler test_sync

clean:
	rm -f ./bin/*.bin ./build/*.o test_security test_scheduler test_file_system test_memory_management test_io test_directories test_disk_scheduler test_sync