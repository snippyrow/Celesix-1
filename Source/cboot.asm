[ORG 0x9800] ; start at first 16MB of ram
[bits 32]

DATA_SEG equ gdt_data-gdt_start


; this is an entirely new system!! this DOES NOT link to the bootloader
cboot:
    cli                     ; Disable interrupts


    ; Set up the stack pointer for protected mode
    mov esp, 0x90000       ; Reset the stack in protected mode
    mov al, 'L'
    mov ah, 0xf
    mov [0xb8000], ax
    ; Load the GDT and switch to protected mode (your existing code)
    lgdt [gdt_descriptor]
    jmp 0x08:Entry16

    

[bits 16]
Entry16:
    cli                     ; Disable interrupts

    ; Set up the data segment
    mov ax, DATA_SEG       ; Load data segment selector
    mov ds, ax             ; Set DS
    mov es, ax             ; Set ES
    mov fs, ax             ; Set FS
    mov gs, ax             ; Set GS
    mov ss, ax             ; Set SS

    ; Disable paging
    mov eax, cr0           ; Read CR0
    and eax, 0xFFFFFFFE    ; Clear the paging bit (bit 31)
    mov cr0, eax           ; Write back to CR0


    ; Jump to real mode
    jmp 0:GoRMode          ; Far jump to set CS

GoRMode:
    ; Set up the stack in real mode
    mov ax, 0              ; Reset segment registers
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax             ; Set SS to 0
    mov sp, 0x7c00        ; Set stack pointer (top of memory)
    lidt [idt_real]

    ; Optionally: clear screen or set up video mode here
    ; (add your video mode code)

    mov ax, 0003h    ; BIOS.SetVideoMode 80x25 16-color text
    int 10h

    mov ah, 0xe
    mov al, 'W'
    int 0x10

    jmp $                  ; Infinite loop to keep it running


; cboot must be the first item loaded into memory.. for obvious reasons
savcr0:
    dd 0

idt_real:
	dw 0x3ff		; 256 entries, 4b each = 1K
	dd 0			; Real Mode IVT @ 0x0000

gdt_start:
    ; null descriptor
    dq 0
gdt_code:
    dw 0xffff ; first part of the limit
    dw 0 ; base address
    db 0 ; base address again
    db 0b10011010 ; access (see GDT documentation)
    db 0b00001111 ; the flags, and then the final part of the 1MB limit
    db 0 ; another part of the base

gdt_data:
    dw 0xffff ; first part of the limit
    dw 0 ; base address
    db 0 ; base address again
    db 0b10010010 ; access (see GDT documentation)
    db 0b00001111 ; the flags, and then the final part of the 1MB limit
    db 0 ; another part of the base
gdt_end:
    
gdt_descriptor:
    ; Size of the GDT in bytes (end - start - 1 for 0-based index)
    dw gdt_end - gdt_start

    ; Address of the GDT
    dd gdt_start



dw 0xbb66