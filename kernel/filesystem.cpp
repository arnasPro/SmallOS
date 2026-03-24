#include "filesystem.h"
#include "memory.h"
#include "console.h"

static Inode inode_table[MAX_FILES];
static FileDescriptor file_table[MAX_FILES];
static uint32_t inode_count = 0;

void FileSystem::init() {
    for(int i = 0; i < MAX_FILES; i++) {
        inode_table[i].inode_num = 0;
        file_table[i].open = false;
    }
    Console::print("[*] File system initialized\n");
}

uint32_t FileSystem::create_file(const char* name) {
    if(inode_count >= MAX_FILES) {
        return 0;  // Inode table full
    }
    
    // Check if file already exists
    for(int i = 0; i < inode_count; i++) {
        if(compare_string_util(inode_table[i].name, name)) {
            return 0;  // File already exists
        }
    }
    
    Inode* new_inode = &inode_table[inode_count];
    new_inode->inode_num = inode_count;
    new_inode->size = 0;
    new_inode->created_time = 0;
    new_inode->modified_time = 0;
    
    // Copy filename
    int i = 0;
    while(name[i] && i < MAX_FILENAME - 1) {
        new_inode->name[i] = name[i];
        i++;
    }
    new_inode->name[i] = '\0';
    
    // Allocate data block
    new_inode->data_block = reinterpret_cast<uint32_t>(Memory::allocate(4096));
    
    inode_count++;
    return new_inode->inode_num;
}

bool FileSystem::delete_file(const char* name) {
    for(int i = 0; i < inode_count; i++) {
        if(compare_string_util(inode_table[i].name, name)) {
            inode_table[i].inode_num = 0;
            return true;
        }
    }
    return false;
}

uint32_t FileSystem::open_file(const char* name, FileMode mode) {
    Inode* inode = nullptr;
    
    // Find inode
    for(int i = 0; i < inode_count; i++) {
        if(compare_string_util(inode_table[i].name, name)) {
            inode = &inode_table[i];
            break;
        }
    }
    
    if(!inode) return 0;  // File not found
    
    // Find free file descriptor
    for(int i = 0; i < MAX_FILES; i++) {
        if(!file_table[i].open) {
            file_table[i].inode_num = inode->inode_num;
            file_table[i].offset = 0;
            file_table[i].mode = mode;
            file_table[i].open = true;
            return i + 1;  // FD 0 is reserved
        }
    }
    
    return 0;  // No free file descriptor
}

void FileSystem::close_file(uint32_t fd) {
    if(fd > 0 && fd <= MAX_FILES) {
        file_table[fd - 1].open = false;
    }
}

uint32_t FileSystem::read_file(uint32_t fd, void* buffer, uint32_t size) {
    if(fd == 0 || fd > MAX_FILES || !file_table[fd - 1].open) {
        return 0;
    }
    
    FileDescriptor* f = &file_table[fd - 1];
    Inode* inode = &inode_table[f->inode_num];
    
    uint32_t to_read = size;
    if(f->offset + size > inode->size) {
        to_read = inode->size - f->offset;
    }
    
    if(to_read > 0) {
        uint8_t* src = reinterpret_cast<uint8_t*>(inode->data_block) + f->offset;
        uint8_t* dst = reinterpret_cast<uint8_t*>(buffer);
        
        for(uint32_t i = 0; i < to_read; i++) {
            dst[i] = src[i];
        }
        
        f->offset += to_read;
    }
    
    return to_read;
}

uint32_t FileSystem::write_file(uint32_t fd, const void* buffer, uint32_t size) {
    if(fd == 0 || fd > MAX_FILES || !file_table[fd - 1].open) {
        return 0;
    }
    
    FileDescriptor* f = &file_table[fd - 1];
    Inode* inode = &inode_table[f->inode_num];
    
    uint8_t* dst = reinterpret_cast<uint8_t*>(inode->data_block) + f->offset;
    const uint8_t* src = reinterpret_cast<const uint8_t*>(buffer);
    
    for(uint32_t i = 0; i < size; i++) {
        dst[i] = src[i];
    }
    
    f->offset += size;
    if(f->offset > inode->size) {
        inode->size = f->offset;
    }
    
    return size;
}

void FileSystem::list_files() {
    Console::print("Files:\n");
    for(int i = 0; i < inode_count; i++) {
        if(inode_table[i].inode_num != 0) {
            Console::print("  ");
            Console::print(inode_table[i].name);
            Console::print(" (");
            Console::print_num(inode_table[i].size);
            Console::print(" bytes)\n");
        }
    }
}

// Utility function
static bool compare_string_util(const char* a, const char* b) {
    while(*a && *b) {
        if(*a != *b) return false;
        a++;
        b++;
    }
    return *a == *b;
}
