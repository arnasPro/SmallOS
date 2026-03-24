.PHONY: all clean build

# Directories
BOOT_DIR = bootloader
KERNEL_DIR = kernel
DRIVER_DIR = drivers

# Compiler flags
CFLAGS = -ffreestanding -Wall -Wextra -g
ASMFLAGS = -f elf32 -g
BOOT_FLAGS = -f bin

# Output files
BOOT_BIN = boot.bin
KERNEL_BIN = kernel.bin
KERNEL_ELF = kernel.elf
DISK_IMG = minios.img

# Object files
KERNEL_OBJS = kernel.o memory.o process.o filesystem.o console.o kernel_entry.o

all: $(DISK_IMG) cleanup_intermediates

# Cleanup intermediate build files
cleanup_intermediates:
	@rm -f *.bin *.elf *.o 2>/dev/null || true
	@echo "Build complete: minios.img"

# Bootloader
boot.bin: $(BOOT_DIR)/boot.asm
	nasm $(BOOT_FLAGS) $< -o $@

# Kernel objects
kernel.o: $(KERNEL_DIR)/kernel.cpp
	g++ $(CFLAGS) -c $< -o $@

memory.o: $(KERNEL_DIR)/memory.cpp
	g++ $(CFLAGS) -c $< -o $@

process.o: $(KERNEL_DIR)/process.cpp
	g++ $(CFLAGS) -c $< -o $@

filesystem.o: $(KERNEL_DIR)/filesystem.cpp
	g++ $(CFLAGS) -c $< -o $@

console.o: $(DRIVER_DIR)/console.cpp
	g++ $(CFLAGS) -c $< -o $@

kernel_entry.o: $(KERNEL_DIR)/kernel_entry.asm
	nasm $(ASMFLAGS) $< -o $@

# Link kernel
$(KERNEL_ELF): $(KERNEL_OBJS)
	ld -m elf_i386 -Ttext 0x10000 -o $@ $^

# Convert to binary
$(KERNEL_BIN): $(KERNEL_ELF)
	objcopy -O binary $< $@

# Create disk image
$(DISK_IMG): $(BOOT_BIN) $(KERNEL_BIN)
	dd if=/dev/zero of=$@ bs=1440k count=1
	dd if=$(BOOT_BIN) of=$@ conv=notrunc
	dd if=$(KERNEL_BIN) of=$@ bs=512 seek=2 conv=notrunc
	@echo "Disk image created: $@"

build: $(DISK_IMG)

clean:
	rm -f *.o *.bin *.elf *.img 2>/dev/null || true
	@echo "Cleaned!"

help:
	@echo "MiniOS Build System"
	@echo "==================="
	@echo "make build   - Build the OS image"
	@echo "make run     - Run OS in QEMU"
	@echo "make clean   - Clean build files"
	@echo "make help    - Show this help"
