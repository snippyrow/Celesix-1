[BITS 16]
[ORG 0x7c00]

KERNEL_LOCATION equ 0x1000
CODE_SEG equ GDT_CODE - GDT_START
DATA_SEG equ GDT_DATA - GDT_START




reset_drive:
    mov al, ah
    mov ah, 0x0E     ; BIOS teletype output function
    add al, 0x30
    int 0x10
    mov al, '#'
    int 0x10

    mov ah, 0
    int 13h
    or ah, ah
    jnz reset_drive

mov bx, KERNEL_LOCATION
mov ax, 0
mov es, ax

mov ah, 2   ; read sectors
mov al, 53  ; total number of sectors to read (max 53)
mov ch, 0   ; cylinder to read (none needed for now)
mov cl, 2   ; Starting sector (1 - 54) (1 is boot)
mov dh, 0   ; The specified head to read from 0-255
mov dl, 0x80; drive number

int 13h
jc reset_drive

add al, 0x30
mov ah, 0xe
int 0x10



mov ah, 0
mov al, 0x3
int 0x10 ; text mode, clear screen

cli
xor ax, ax
mov ds, ax ; clear ds
lgdt [GDT_DESC]

mov eax, cr0
or eax, 1
mov cr0, eax ; entering protected mode


jmp CODE_SEG:PROTECTED_START ; far-jump to clear instructions, begin

db "123456789ABCDEFGHIJKLMNOP"

GDT_START:
    GDT_NULL:
        dq 0x0
    GDT_CODE:
        dw 0xffff
        dw 0x0
        db 0x0
        db 0b10011010
        db 0b11001111
        db 0x0
    GDT_DATA:
        dw 0xffff
        dw 0x0
        db 0x0
        db 0b10010010
        db 0b11001111
        db 0x0
GDT_END:

GDT_DESC:
    dw GDT_END-GDT_START
    dd GDT_START



[BITS 32]
PROTECTED_START:
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov esp, 0x90000 ; set stack far away

    ; test print
    call CODE_SEG:KERNEL_LOCATION

    jmp $


times 510-($-$$) db 0
dw 0xaa55