#include "console.h"

#define VGA_MEMORY 0xB8000
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

static uint16_t* video_buffer = reinterpret_cast<uint16_t*>(VGA_MEMORY);
static uint8_t current_color = (Console::WHITE << 4) | Console::BLACK;
static uint32_t cursor_x = 0;
static uint32_t cursor_y = 0;

void Console::init() {
    clear();
}

void Console::set_color(Color fg, Color bg) {
    current_color = (bg << 4) | fg;
}

void Console::clear() {
    uint16_t clear_char = (' ' << 8) | current_color;
    
    for(int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        video_buffer[i] = clear_char;
    }
    
    cursor_x = 0;
    cursor_y = 0;
}

void Console::print(const char* str) {
    while(*str) {
        if(*str == '\n') {
            cursor_x = 0;
            cursor_y++;
        }
        else if(*str == '\r') {
            cursor_x = 0;
        }
        else if(*str == '\t') {
            cursor_x += 4;
        }
        else {
            print_char(*str);
        }
        
        if(cursor_y >= SCREEN_HEIGHT) {
            cursor_y = 0;
        }
        
        str++;
    }
}

void Console::print_char(char c) {
    uint32_t index = cursor_y * SCREEN_WIDTH + cursor_x;
    video_buffer[index] = (c << 8) | current_color;
    
    cursor_x++;
    if(cursor_x >= SCREEN_WIDTH) {
        cursor_x = 0;
        cursor_y++;
    }
}

void Console::print_num(uint32_t num) {
    if(num == 0) {
        print_char('0');
        return;
    }
    
    char buffer[11];
    int i = 0;
    
    while(num > 0) {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }
    
    for(int j = i - 1; j >= 0; j--) {
        print_char(buffer[j]);
    }
}

void Console::print_hex(uint32_t num) {
    print("0x");
    
    for(int i = 28; i >= 0; i -= 4) {
        uint8_t digit = (num >> i) & 0xF;
        if(digit < 10) {
            print_char('0' + digit);
        } else {
            print_char('A' + digit - 10);
        }
    }
}

char Console::read_char() {
    // Simplified: would need actual keyboard driver
    // For now, return null character
    return '\0';
}
