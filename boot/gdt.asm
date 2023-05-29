[bits 16]
GDT_start:
    GDT_null:
        dd 0 ; Four times 00000000
        dd 0 ; Four times 00000000

    ; CODE SEGMENT
    ;
    ; base  = 0x0000
    ; limit = 0xfffff
    ;
    ; p,p,t: 1001
    ;   present   = 1  (yes, it is used)
    ;   privilege = 00 (highest privilege)
    ;   type      = 1  (code/data segment)
    ;
    ; type flags: 1010
    ;   code seg   = 1
    ;   conforming = 0
    ;   readable   = 1
    ;   accessed   = 0
    ;
    ; other flags: 1100
    ;   up limit = 1 (increase memory available)
    ;   32 bits  = 1
    ;   64 bits  = 0
    ;   AVL      = 0

    GDT_code:
        ; limit (first 16 bits)
        dw 0xffff

        ; First 24 bits of base
        dw 0 ; 16 bits
        db 0 ; 8 bits

        ; p,p,t, type flags
        db 0b10011010

        ; other flags, limit (last 4 bits)
        db 0b11001111

        ; last 8 bits of the base
        db 0

    ; DATA SEGMENT
    ;
    ; base  = 0x0000
    ; limit = 0xfffff
    ;
    ; p,p,t: 1001
    ;   present   = 1  (yes, it is used)
    ;   privilege = 00 (highest privilege)
    ;   type      = 1  (code/data segment)
    ;
    ; type flags: 0010
    ;   code seg  = 0
    ;   direction = 0 (normal memory direction)
    ;   writeable = 1
    ;   accessed  = 0
    ;
    ; other flags: 1100
    ;   up limit = 1 (increase memory available)
    ;   32 bits  = 1
    ;   64 bits  = 0
    ;   AVL      = 0

    GDT_data:
        ; limit (first 16 bits)
        dw 0xffff

        ; First 24 bits of base
        dw 0 ; 16 bits
        db 0 ; 8 bits

        ; p,p,t, type flags
        db 0b10010010

        ; other flags, limit (last 4 bits)
        db 0b11001111

        ; last 8 bits of the base
        db 0
GDT_end:

GDT_descriptor:
    dw GDT_end - GDT_start - 1 ; size
    dd GDT_start               ; start

; Segment locations in memory
CODE_SEG equ GDT_code - GDT_start
DATA_SEG equ GDT_data - GDT_start