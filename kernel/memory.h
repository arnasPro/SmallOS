#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

namespace Memory {
    // Physical memory starts at 0x100000 (1MB)
    const uint32_t HEAP_START = 0x100000;
    const uint32_t HEAP_SIZE = 64 * 1024 * 1024;  // 64 MB
    
    struct MemoryBlock {
        uint32_t size;
        bool free;
        MemoryBlock* next;
    };
    
    void init();
    void* allocate(uint32_t size);
    void deallocate(void* ptr);
    uint32_t get_used();
    void print_memory_map();
}

#endif
