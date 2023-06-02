[org 0x7c00]
STACK_OFFSET equ 0x7c00
KERNEL_OFFSET equ 0x7e00

[bits 16]
start:
    ; Disable interrupts
    cli

    ; Setup flat memory model (segments start at zero)
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Setup stack
    mov ss, ax
    mov sp, STACK_OFFSET

    ; Re-enable interrupts
    sti

    ; Load kernel to memory
    mov ah, 0x42
    mov si, disk_packet
    int 0x13
    jc disk_error

    ; Set VGA mode 13h (320x200, 256 colors)
    mov ah, 0x00
    mov al, 0x13
    int 0x10

    ; Disable interrupts
    cli

    ; Load GDT
    lgdt [GDT_descriptor]

    ; Enable protected mode
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax

    ; Setup protected mode
    jmp CODE_SEG:init_32_bit

[bits 32]
init_32_bit:
    ; Adjust segments
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Set stack
    mov esp, STACK_OFFSET

    ; Jump to kernel
    jmp KERNEL_OFFSET

%include "print.asm"
%include "gdt.asm"

; Layout is from here:
; https://en.wikipedia.org/wiki/INT_13H#INT_13h_AH=42h:_Extended_Read_Sectors_From_Drive
disk_packet:
    db 0x10
    db 0
.num_sectors:
    dw 0x20 ; How many sectors to load
.offset:
    dd KERNEL_OFFSET ; Where to load kernel in memory
.read_from:
    dq 1 ; Sector to start reading from

disk_error:
    mov si, disk_error_msg
    call print
    jmp hang

hang:
    jmp $

welcome_msg: db 'Hello, world!', 0
disk_error_msg: db 'Disk error', 0

; Zero padding + magic number
times 510-($-$$) db 0
dw 0xaa55