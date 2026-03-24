@echo off
REM MiniOS Build Script for Windows

setlocal enabledelayedexpansion

echo Building MiniOS...
echo ==================

if "%1"=="clean" goto clean
if "%1"=="run" goto run
if "%1"=="help" goto help

:build
echo [1/7] Assembling bootloader...
nasm -f bin bootloader/boot.asm -o boot.bin
if errorlevel 1 goto error

echo [2/7] Compiling kernel...
g++ -ffreestanding -c kernel/kernel.cpp -o kernel.o
if errorlevel 1 goto error

echo [3/7] Compiling memory manager...
g++ -ffreestanding -c kernel/memory.cpp -o memory.o
if errorlevel 1 goto error

echo [4/7] Compiling process manager...
g++ -ffreestanding -c kernel/process.cpp -o process.o
if errorlevel 1 goto error

echo [5/7] Compiling file system...
g++ -ffreestanding -c kernel/filesystem.cpp -o filesystem.o
if errorlevel 1 goto error

echo [6/7] Compiling console driver...
g++ -ffreestanding -c drivers/console.cpp -o console.o
if errorlevel 1 goto error

echo [7/7] Assembling kernel entry point...
nasm -f elf32 kernel/kernel_entry.asm -o kernel_entry.o
if errorlevel 1 goto error

echo Linking kernel...
ld -m elf_i386 -Ttext 0x10000 -o kernel.elf kernel_entry.o kernel.o memory.o process.o filesystem.o console.o
if errorlevel 1 goto error

echo Converting to binary...
objcopy -O binary kernel.elf kernel.bin
if errorlevel 1 goto error

echo Creating disk image...
dd if=/dev/zero of=minios.img bs=1440k count=1 2>nul
dd if=boot.bin of=minios.img conv=notrunc 2>nul
dd if=kernel.bin of=minios.img bs=512 seek=2 conv=notrunc 2>nul

echo.
echo ===========================
echo Build successful!
echo Run with: build.bat run
echo ===========================
goto end

:run
if not exist minios.img (
    echo Error: minios.img not found. Build first with: build.bat
    goto error
)
echo Starting QEMU...
qemu-system-i386 -fda minios.img -monitor stdio
goto end

:clean
echo Cleaning build files...
del /q *.bin *.elf *.img *.o 2>nul
echo Cleaned!
goto end

:help
echo MiniOS Build Script
echo ===================
echo Usage: build.bat [command]
echo.
echo Commands:
echo   (no args)  - Build the OS
echo   run        - Build and run in QEMU
echo   clean      - Remove build files
echo   help       - Show this help
goto end

:error
echo.
echo Build FAILED!
echo Check that you have:
echo   - NASM (assembler)
echo   - GCC/G++ (C++ compiler)
echo   - GNU binutils (ld, objcopy)
echo   - dd (disk utility, usually comes with MSYS2 or Git Bash)
echo   - QEMU (for running)
exit /b 1

:end
