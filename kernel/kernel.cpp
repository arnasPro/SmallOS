// MiniOS Kernel - Main kernel code
#include "kernel.h"
#include "memory.h"
#include "process.h"
#include "filesystem.h"
#include "console.h"

extern "C" void kernel_main() {
    Console::clear();
    Console::set_color(Console::GREEN, Console::BLACK);
    Console::print("====================================\n");
    Console::print("  MiniOS Kernel v1.0\n");
    Console::print("====================================\n");
    Console::set_color(Console::WHITE, Console::BLACK);
    
    // Initialize memory management
    Console::print("\n[*] Initializing memory management...\n");
    Memory::init();
    
    // Initialize file system
    Console::print("[*] Initializing file system...\n");
    FileSystem::init();
    
    // Initialize process management
    Console::print("[*] Initializing process management...\n");
    ProcessManager::init();
    
    Console::set_color(Console::GREEN, Console::BLACK);
    Console::print("\n[*] System ready!\n\n");
    Console::set_color(Console::WHITE, Console::BLACK);
    
    // Start shell
    shell_main();
    
    // Halt system
    while(1) {
        asm volatile("hlt");
    }
}

void shell_main() {
    char buffer[256];
    uint32_t buffer_idx = 0;
    
    Console::print("MiniOS> ");
    
    while(1) {
        // Read keyboard input (simplified - would need proper keyboard driver)
        char ch = Console::read_char();
        
        if(ch == '\n') {
            Console::print("\n");
            buffer[buffer_idx] = '\0';
            
            // Execute command
            if(buffer_idx > 0) {
                execute_command(buffer);
            }
            
            Console::print("MiniOS> ");
            buffer_idx = 0;
        } 
        else if(ch == '\b' && buffer_idx > 0) {
            // Backspace
            buffer_idx--;
            Console::print("\b \b");
        }
        else if(buffer_idx < 255) {
            buffer[buffer_idx++] = ch;
            Console::print_char(ch);
        }
    }
}

void execute_command(const char* cmd) {
    if(compare_string(cmd, "help")) {
        Console::print("Available commands:\n");
        Console::print("  help     - Show this help\n");
        Console::print("  clear    - Clear screen\n");
        Console::print("  memory   - Show memory info\n");
        Console::print("  ps       - List processes\n");
        Console::print("  echo     - Echo text\n");
        Console::print("  reboot   - Reboot system\n");
    }
    else if(compare_string(cmd, "clear")) {
        Console::clear();
    }
    else if(compare_string(cmd, "memory")) {
        Console::print("Total Memory: 64 MB\n");
        Console::print("Used Memory: ");
        Console::print_num(Memory::get_used());
        Console::print(" KB\n");
    }
    else if(compare_string(cmd, "ps")) {
        ProcessManager::list_processes();
    }
    else if(compare_string(cmd, "echo") || starts_with(cmd, "echo ")) {
        const char* text = cmd + 5;
        Console::print(text);
        Console::print("\n");
    }
    else if(compare_string(cmd, "reboot")) {
        Console::print("Rebooting...\n");
        reboot();
    }
    else {
        Console::print("Unknown command: ");
        Console::print(cmd);
        Console::print("\n");
    }
}

bool compare_string(const char* a, const char* b) {
    while(*a && *b) {
        if(*a != *b) return false;
        a++;
        b++;
    }
    return *a == *b;
}

bool starts_with(const char* str, const char* prefix) {
    while(*prefix) {
        if(*str != *prefix) return false;
        str++;
        prefix++;
    }
    return true;
}

void reboot() {
    // Triple fault reboot
    uint32_t zero = 0;
    asm volatile("lidt %0" : : "m"(zero));
    asm volatile("int $3");
}
