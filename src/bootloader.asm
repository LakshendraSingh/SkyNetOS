[BITS 16]            
[ORG 0x7C00]        

CODE_OFFSET equ 0x8
DATA_OFFSET equ 0x10


KERNEL_LOAD_SEG equ 0x1000
KERNEL_START_ADDR equ 0x10000

start:
    mov [BOOT_DRIVE], dl ; Store boot drive

    cli             ;disable interrupts

    ;stack Initialization
    xor ax, ax 
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    sti             ;enable interrupts

    ;load kernel
    mov ax, KERNEL_LOAD_SEG
    mov es, ax
    mov bx, 0x0000

    mov dh, 0x00     ;head 0
    mov dl, [BOOT_DRIVE]
    mov ch, 0x00     ;cylinder 0
    mov cl, 0x02     ;sector 2
    mov ah, 0x02
    mov al, 127      ; Read 127 sectors (full image minus boot sector)
    int 0x13
    jc disk_error

    ; Transition to protected mode
    jmp load_pm

disk_error:
    mov si, DISK_ERR_MSG
    call print_string
    jmp $

print_string:
    mov ah, 0x0e
.loop:
    lodsb
    test al, al
    jz .done
    int 0x10
    jmp .loop
.done:
    ret

DISK_ERR_MSG db "Disk read error!", 0
BOOT_DRIVE db 0

;protected mode
load_pm:
    cli
    lgdt[gdt_descriptor]
    mov eax, cr0
    or al, 1
    mov cr0, eax
    jmp CODE_OFFSET:PModeMain


;GDT

gdt_start:
    dd 0x00000000
    dd 0x00000000
    
    ;code segment descriptor

    dw 0xFFFF           ;limit
    dw 0x0000           ;base
    db 0x00             ;base
    db 10011010b        ;access byte
    db 11001111b        ;flags
    db 0x00             ; base

    ;data segment descriptor

    dw 0xFFFF           ;limit
    dw 0x0000           ;base
    db 0x00             ;base
    db 10010010b        ;access byte
    db 11001111b        ;flags
    db 0x00             ;base

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1 ;gdt size - 1
    dd gdt_start

[BITS 32]
PModeMain:
    mov ax, DATA_OFFSET
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov ss, ax
    mov gs, ax
    mov ebp, 0x9C00
    mov esp, ebp

    in al, 0x92
    or al, 2
    out 0x92, al

    jmp CODE_OFFSET:KERNEL_START_ADDR



times 510 - ($ - $$) db 0    ;Fill the rest of the sector with zeros
dw 0xAA55                   ;Boot signature
