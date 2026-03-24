#include "process.h"
#include "memory.h"
#include "console.h"

static ProcessControlBlock process_table[MAX_PROCESSES];
static uint32_t next_pid = 1;
static uint32_t current_pid = 0;

void ProcessManager::init() {
    // Initialize process table
    for(int i = 0; i < MAX_PROCESSES; i++) {
        process_table[i].pid = 0;
        process_table[i].state = PROCESS_TERMINATED;
    }
    
    // Create idle process
    ProcessControlBlock* idle = &process_table[0];
    idle->pid = 0;
    idle->name = "idle";
    idle->state = PROCESS_READY;
    idle->priority = 0;
    
    Console::print("[*] Process manager initialized\n");
}

uint32_t ProcessManager::create_process(const char* name, void (*entry_point)()) {
    for(int i = 0; i < MAX_PROCESSES; i++) {
        if(process_table[i].state == PROCESS_TERMINATED) {
            ProcessControlBlock* proc = &process_table[i];
            proc->pid = next_pid++;
            proc->name = name;
            proc->state = PROCESS_READY;
            proc->priority = 10;
            
            // Allocate stack for process
            uint8_t* stack = 
                reinterpret_cast<uint8_t*>(Memory::allocate(4096));  // 4KB stack
            proc->stack_ptr = reinterpret_cast<uint32_t>(stack) + 4096;
            
            return proc->pid;
        }
    }
    return 0;  // No free process slot
}

void ProcessManager::terminate_process(uint32_t pid) {
    for(int i = 0; i < MAX_PROCESSES; i++) {
        if(process_table[i].pid == pid) {
            process_table[i].state = PROCESS_TERMINATED;
            return;
        }
    }
}

void ProcessManager::list_processes() {
    Console::print("PID\tName\t\tState\n");
    Console::print("---\t----\t\t-----\n");
    
    for(int i = 0; i < MAX_PROCESSES; i++) {
        if(process_table[i].pid != 0) {
            Console::print_num(process_table[i].pid);
            Console::print("\t");
            Console::print(process_table[i].name);
            Console::print("\t\t");
            
            switch(process_table[i].state) {
                case PROCESS_READY:
                    Console::print("READY\n");
                    break;
                case PROCESS_RUNNING:
                    Console::print("RUNNING\n");
                    break;
                case PROCESS_BLOCKED:
                    Console::print("BLOCKED\n");
                    break;
                case PROCESS_TERMINATED:
                    Console::print("TERMINATED\n");
                    break;
            }
        }
    }
}

ProcessControlBlock* ProcessManager::get_current_process() {
    if(current_pid < MAX_PROCESSES) {
        return &process_table[current_pid];
    }
    return nullptr;
}

void ProcessManager::schedule() {
    // Simple round-robin scheduler
    current_pid = (current_pid + 1) % MAX_PROCESSES;
}
