#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>

#define MAX_PROCESSES 32

enum ProcessState {
    PROCESS_READY,
    PROCESS_RUNNING,
    PROCESS_BLOCKED,
    PROCESS_TERMINATED
};

struct ProcessControlBlock {
    uint32_t pid;
    ProcessState state;
    uint32_t priority;
    uint32_t stack_ptr;
    uint32_t page_dir;  // Page directory address
    const char* name;
};

namespace ProcessManager {
    void init();
    uint32_t create_process(const char* name, void (*entry_point)());
    void terminate_process(uint32_t pid);
    void list_processes();
    ProcessControlBlock* get_current_process();
    void schedule();
}

#endif
