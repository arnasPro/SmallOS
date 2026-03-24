; Kernel Entry Point (32-bit)
; This transitions from the bootloader to C++ kernel code

BITS 32

extern kernel_main

section .text
global _start
_start:
    ; Setup stack
    mov esp, 0x90000
    mov ebp, esp
    
    ; Call kernel_main
    call kernel_main
    
    ; Hang system
    jmp $

section .data
align 0x1000
kernel_stack:
    resb 0x4000  ; 16KB kernel stack
kernel_stack_top:
