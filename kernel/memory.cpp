#include "memory.h"
#include "console.h"

static Memory::MemoryBlock* heap_start = nullptr;
static uint32_t total_allocated = 0;

void Memory::init() {
    heap_start = reinterpret_cast<Memory::MemoryBlock*>(HEAP_START);
    heap_start->size = HEAP_SIZE - sizeof(MemoryBlock);
    heap_start->free = true;
    heap_start->next = nullptr;
}

void* Memory::allocate(uint32_t size) {
    MemoryBlock* current = heap_start;
    
    // First-fit allocation
    while(current) {
        if(current->free && current->size >= size + sizeof(MemoryBlock)) {
            // Split the block
            MemoryBlock* new_block = 
                reinterpret_cast<MemoryBlock*>(
                    reinterpret_cast<uint8_t*>(current) + 
                    sizeof(MemoryBlock) + size
                );
            
            new_block->size = current->size - size - sizeof(MemoryBlock);
            new_block->free = true;
            new_block->next = current->next;
            
            current->size = size;
            current->free = false;
            current->next = new_block;
            
            total_allocated += size;
            return reinterpret_cast<void*>(
                reinterpret_cast<uint8_t*>(current) + sizeof(MemoryBlock)
            );
        }
        current = current->next;
    }
    
    return nullptr;
}

void Memory::deallocate(void* ptr) {
    if(!ptr) return;
    
    MemoryBlock* block = 
        reinterpret_cast<MemoryBlock*>(
            reinterpret_cast<uint8_t*>(ptr) - sizeof(MemoryBlock)
        );
    
    block->free = true;
    total_allocated -= block->size;
    
    // Coalesce adjacent free blocks
    MemoryBlock* current = heap_start;
    while(current && current->next) {
        if(current->free && current->next->free) {
            current->size += sizeof(MemoryBlock) + current->next->size;
            current->next = current->next->next;
        }
        current = current->next;
    }
}

uint32_t Memory::get_used() {
    return total_allocated / 1024;  // Return in KB
}

void Memory::print_memory_map() {
    Console::print("Memory Map:\n");
    MemoryBlock* current = heap_start;
    uint32_t block_num = 0;
    
    while(current) {
        Console::print("Block ");
        Console::print_num(block_num++);
        Console::print(": ");
        Console::print_num(current->size);
        Console::print(" bytes - ");
        Console::print(current->free ? "FREE" : "ALLOCATED");
        Console::print("\n");
        current = current->next;
    }
}
