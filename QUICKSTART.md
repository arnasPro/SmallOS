# MiniOS Quick Start Guide

## Overview
**MiniOS** is a minimal educational operating system demonstrating fundamental OS concepts including:
- Real mode → Protected mode transition
- Memory management with heap allocation
- Process management and scheduling
- Simple file system
- Interactive shell interface

## System Requirements
- **NASM** - Netwide Assembler (for x86 assembly)
- **GCC/G++** - GNU Compiler Collection
- **GNU Binutils** - ld, objcopy for linking
- **QEMU** - x86 emulator (for running)

### Windows Installation
```bash
# Using MSYS2
pacman -S nasm gcc binutils qemu-system-x86

# Or download individually:
# - NASM: https://www.nasm.us/
# - MinGW-w64: https://www.mingw-w64.org/
# - QEMU: https://www.qemu.org/
```

## Quick Build

From the `MiniOS` directory:

```bash
# Build the OS
make build

# Run in QEMU
make run

# Clean build files
make clean
```

## Manual Build Steps

If `make` is not available:

```bash
# 1. Assemble bootloader
nasm -f bin bootloader/boot.asm -o boot.bin

# 2. Compile kernel
g++ -ffreestanding -c kernel/kernel.cpp -o kernel.o
g++ -ffreestanding -c kernel/memory.cpp -o memory.o
g++ -ffreestanding -c kernel/process.cpp -o process.o
g++ -ffreestanding -c kernel/filesystem.cpp -o filesystem.o
g++ -ffreestanding -c drivers/console.cpp -o console.o

# 3. Assemble kernel entry
nasm -f elf32 kernel/kernel_entry.asm -o kernel_entry.o

# 4. Link kernel
ld -m elf_i386 -Ttext 0x10000 -o kernel.elf kernel_entry.o kernel.o memory.o process.o filesystem.o console.o

# 5. Convert to binary
objcopy -O binary kernel.elf kernel.bin

# 6. Create disk image
dd if=/dev/zero of=minios.img bs=1440k count=1
dd if=boot.bin of=minios.img conv=notrunc
dd if=kernel.bin of=minios.img bs=512 seek=2 conv=notrunc

# 7. Run
qemu-system-i386 -fda minios.img
```

## Using the OS

Once booted, you'll see the MiniOS shell:
```
MiniOS>
```

### Available Commands

| Command | Description |
|---------|-------------|
| `help` | Show all available commands |
| `clear` | Clear the screen |
| `memory` | Display memory usage statistics |
| `ps` | List all processes and their states |
| `echo <text>` | Echo text to the screen |
| `reboot` | Reboot the system |

### Example Session

```
MiniOS> help
Available commands:
  help     - Show this help
  clear    - Clear screen
  memory   - Show memory info
  ps       - List processes
  echo     - Echo text
  reboot   - Reboot system

MiniOS> memory
Total Memory: 64 MB
Used Memory: 0 KB

MiniOS> ps
PID	Name		State
---	----		-----
0	idle		READY

MiniOS> echo Hello MiniOS!
Hello MiniOS!

MiniOS>
```

## Architecture

### Memory Layout (x86)
```
0x00000000 +------------------+
           | Real Mode BIOS    |
0x00007C00 +------------------+
           | Bootloader (512B) |
0x00007E00 +------------------+
           | Kernel (loaded)   |
0x00010000 +------------------+
           | Kernel Code/Data  |
0x00090000 +------------------+
           | Kernel Stack      |
0x00100000 +------------------+
           | Heap (64 MB)      |
0x04000000 +------------------+
```

### Boot Process
1. **BIOS** loads bootloader from disk sector 0
2. **Bootloader** (~512 bytes)
   - Initializes CPU to protected mode
   - Loads GDT (Global Descriptor Table)
   - Reads kernel from disk (sector 2+)
   - Jumps to kernel entry point
3. **Kernel** (~30 KB)
   - Initializes memory manager
   - Initializes file system
   - Initializes process manager
   - Launches interactive shell

## Code Structure

### Bootloader (`bootloader/boot.asm`)
- Real mode → Protected mode transition
- Disk I/O for loading kernel
- GDT setup for x86 protection

### Kernel Core (`kernel/kernel.cpp`)
- Main entry point
- Shell implementation
- Command parsing and execution

### Memory Manager (`kernel/memory.cpp`)
- Heap allocation/deallocation (first-fit)
- Block coalescing for defragmentation
- Simple memory tracking

### Process Manager (`kernel/process.cpp`)
- Process control blocks (PCB)
- Basic round-robin scheduler
- Process state management

### File System (`kernel/filesystem.cpp`)
- Inode-based file storage
- File creation/deletion/read/write
- Simple in-memory file table

### Console Driver (`drivers/console.cpp`)
- VGA text mode (80x25 characters)
- 16-color support
- String and number formatting

## Learning Resources

This OS demonstrates:
- **Real Mode** vs **Protected Mode**
- **Segment Registers** and GDT
- **Memory Segmentation**
- **Interrupt Handling** (basic structure)
- **Process Scheduling**
- **Heap Management**
- **File System Design**

## Limitations & Future Enhancements

Current limitations:
- No paging (no virtual memory)
- No keyboard driver (shell is simulated)
- Limited file system (in-memory only)
- No multi-tasking with context switching
- No interrupt handlers beyond basic structure

Possible enhancements:
- [ ] Paging support for virtual memory
- [ ] Proper interrupt handlers (IDT)
- [ ] Keyboard driver integration
- [ ] Disk-based file system (FAT-like)
- [ ] Context switching for multi-tasking
- [ ] User mode vs kernel mode
- [ ] Virtual memory management
- [ ] Basic network stack

## Debugging Tips

### Using QEMU Monitor
When QEMU is running, you can access the monitor with `Ctrl+Alt+2`:
```
(qemu) info mem
(qemu) info registers
(qemu) quit
```

### Analyzing Kernel
```bash
# View kernel symbols
objdump -t kernel.elf | grep kernel

# Disassemble kernel
objdump -d kernel.elf

# View kernel sections
readelf -S kernel.elf
```

## References

- **x86 Architecture**: Intel x86 Instruction Set Reference
- **OS Design**: "Operating Systems: Three Easy Pieces" by Remzi Arpaci-Dusseau
- **Assembly**: "Art of Assembly Language" by Randall Hyde
- **QEMU**: https://www.qemu.org/documentation/

---

**Happy OS development! 🚀**

This is an educational project - not suitable for production use. Have fun learning!
