#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stdint.h>

#define MAX_FILES 32
#define MAX_FILENAME 32

enum FileMode {
    FILE_READ = 1,
    FILE_WRITE = 2,
    FILE_APPEND = 4
};

struct Inode {
    uint32_t inode_num;
    uint32_t size;
    uint32_t created_time;
    uint32_t modified_time;
    char name[MAX_FILENAME];
    uint32_t data_block;  // Simplified: single data block per file
};

struct FileDescriptor {
    uint32_t inode_num;
    uint32_t offset;
    FileMode mode;
    bool open;
};

namespace FileSystem {
    void init();
    uint32_t create_file(const char* name);
    bool delete_file(const char* name);
    uint32_t open_file(const char* name, FileMode mode);
    void close_file(uint32_t fd);
    uint32_t read_file(uint32_t fd, void* buffer, uint32_t size);
    uint32_t write_file(uint32_t fd, const void* buffer, uint32_t size);
    void list_files();
}

#endif
