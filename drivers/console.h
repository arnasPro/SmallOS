#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdint.h>

namespace Console {
    enum Color {
        BLACK = 0,
        BLUE = 1,
        GREEN = 2,
        CYAN = 3,
        RED = 4,
        MAGENTA = 5,
        BROWN = 6,
        LIGHT_GRAY = 7,
        DARK_GRAY = 8,
        LIGHT_BLUE = 9,
        LIGHT_GREEN = 10,
        LIGHT_CYAN = 11,
        LIGHT_RED = 12,
        LIGHT_MAGENTA = 13,
        YELLOW = 14,
        WHITE = 15
    };
    
    void init();
    void set_color(Color fg, Color bg);
    void clear();
    void print(const char* str);
    void print_char(char c);
    void print_num(uint32_t num);
    void print_hex(uint32_t num);
    char read_char();
}

#endif
