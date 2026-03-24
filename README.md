# MiniOS Build System

## Build Commands

### Build Bootloader
```
nasm -f bin bootloader/boot.asm -o boot.bin
```

### Build Kernel
```
# Compile C++ files
g++ -ffreestanding -c kernel/kernel.cpp -o kernel.o
g++ -ffreestanding -c kernel/memory.cpp -o memory.o
g++ -ffreestanding -c kernel/process.cpp -o process.o
g++ -ffreestanding -c kernel/filesystem.cpp -o filesystem.o
g++ -ffreestanding -c drivers/console.cpp -o console.o

# Assemble kernel entry
nasm -f elf32 kernel/kernel_entry.asm -o kernel_entry.o

# Link kernel
ld -m elf_i386 -Ttext 0x10000 -o kernel.elf kernel_entry.o kernel.o memory.o process.o filesystem.o console.o

# Convert to binary
objcopy -O binary kernel.elf kernel.bin
```

### Create Disk Image
```
# Create 1.44MB floppy image
dd if=/dev/zero of=minios.img bs=1440k count=1

# Write bootloader to sector 0
dd if=boot.bin of=minios.img conv=notrunc

# Write kernel to sector 2
dd if=kernel.bin of=minios.img bs=512 seek=2 conv=notrunc
```

### Run in QEMU
```
qemu-system-i386 -fda minios.img -monitor stdio
```

## Project Structure

```
MiniOS/
├── bootloader/
│   └── boot.asm          # x86 bootloader (loads kernel)
├── kernel/
│   ├── kernel.cpp        # Main kernel + shell
│   ├── kernel.h
│   ├── kernel_entry.asm  # 32-bit kernel entry point
│   ├── memory.cpp        # Memory management (heap)
│   ├── memory.h
│   ├── process.cpp       # Process management
│   ├── process.h
│   ├── filesystem.cpp    # Simple file system
│   └── filesystem.h
├── drivers/
│   ├── console.cpp       # VGA console driver
│   └── console.h
├── fs/                   # File system data (unused)
└── Makefile
```

## Features Implemented

1. **Bootloader**
   - Real mode -> Protected mode transition
   - Loading kernel from disk
   - GDT setup

2. **Kernel**
   - Entry point and initialization
   - Interactive shell with command support

3. **Memory Management**
   - Heap allocation/deallocation
   - First-fit strategy
   - Block coalescing

4. **Process Management**
   - Process control blocks
   - Simple round-robin scheduler
   - Process state tracking

5. **File System**
   - Inode-based file system
   - File creation/deletion
   - File read/write operations

6. **Console Driver**
   - VGA text mode (80x25)
   - Color support
   - String/number printing

## Shell Commands

- `help` - Show available commands
- `clear` - Clear screen
- `memory` - Display memory info
- `ps` - List processes
- `echo <text>` - Echo text to screen
- `reboot` - Reboot system

## Building & Running

This OS is designed to run on x86 architecture. Use QEMU emulator:

1. Install NASM (assembler) and QEMU
2. Run build commands above
3. Launch with: `qemu-system-i386 -fda minios.img`

## Next Steps to Enhance

- Keyboard driver integration
- Paging support for virtual memory
- More complex file system (FAT-like)
- Interrupt handlers
- DMA support
- Network stack basics
- Multi-tasking with context switching

---
This is an educational OS demonstrating core OS concepts!
