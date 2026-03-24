#ifndef KERNEL_H
#define KERNEL_H

extern "C" void kernel_main();
void shell_main();
void execute_command(const char* cmd);
bool compare_string(const char* a, const char* b);
bool starts_with(const char* str, const char* prefix);
void reboot();

#endif
