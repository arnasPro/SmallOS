BITS 16
ORG 0x7C00

; Boot sector entry point
start:
    cli                         
    mov ax, 0                    
    mov ss, ax
    mov sp, 0x7C00
    mov ds, ax
    mov es, ax
    sti                          
    
    
    mov si, boot_msg
    call print_string
    
    
    xor ax, ax
    int 0x13
    
    
    mov ax, 0x1000              ; Load kernel to 0x1000:0x0000
    mov es, ax
    xor bx, bx
    
    mov ah, 0x02                
    mov al, 15                 
    mov ch, 0                   
    mov cl, 2                   
    mov dh, 0                  
    mov dl, 0x80                
    int 0x13
    
    jc disk_error
    
    ; Move to protected mode
    lgdt [gdt_descriptor]
    
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    
    ; Far jump to 32-bit kernel
    jmp 0x0008:kernel_start32
    
disk_error:
    mov si, error_msg
    call print_string
    jmp $

print_string:
    lodsb
    cmp al, 0
    je .done
    mov ah, 0x0E
    int 0x10
    jmp print_string
.done:
    ret

boot_msg: db 'MiniOS Bootloader v1.0', 0x0D, 0x0A, 'Loading kernel...', 0
error_msg: db 'Disk read error!', 0

; GDT Setup
gdt_start:
    dq 0                        
    
    ; Code segment
    dw 0xFFFF                   ; Limit 0-15
    dw 0x0000                   ; Base 0-15
    db 0x00                     ; Base 16-23
    db 0x9A                     ; Access byte
    db 0xCF                     ; Flags & Limit 16-19
    db 0x00                     ; Base 24-31
    
    ; Data segment
    dw 0xFFFF                   ; Limit 0-15
    dw 0x0000                   ; Base 0-15
    db 0x00                     ; Base 16-23
    db 0x92                     ; Access byte
    db 0xCF                     ; Flags & Limit 16-19
    db 0x00                     ; Base 24-31

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

; 32-bit kernel entry point
BITS 32
kernel_start32:
    mov eax, 0x10
    mov ds, eax
    mov es, eax
    mov ss, eax
    mov esp, 0x90000
    
    ; Jump to kernel main
    jmp 0x10000

times 510 - ($ - $$) db 0
dw 0xAA55
