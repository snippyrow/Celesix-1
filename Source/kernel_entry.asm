[bits 32]
[extern kmain]

DATA_SEG equ gdt_data-gdt_start


; declare all ISRs/IRQs for C
call kmain
jmp $

[global isr0]
[global isr8]
[global irqkbd]
[extern isr_ghandler]
[extern kbd_handler]

[global set_VGA]

isr_common_stub:
    call isr_ghandler
    add esp, 8
    iret

irq_common_stub:
    call kbd_handler
    ;add esp, 8
    iret

; from here we have stuff used in the IRQ/ISR routines.
isr0:
    push byte 0
    push byte 0 ; int no
    jmp isr_common_stub

isr8:
    push byte 0
    push byte 8
    jmp isr_common_stub

irqkbd:
    ;push byte 0
    ;push byte 0
    jmp irq_common_stub


set_VGA:
    mov [savesp], esp
    cli
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


    mov ax, 0x0013       ; Set video mode 13h
    int 0x10             ; Call BIOS video interrupt

    ; now go back to protected mode
    cli            ; disable interrupts
    lgdt [GDT_DESC]    ; load GDT register with start address of Global Descriptor Table
    mov eax, cr0 
    or al, 1       ; set PE (Protection Enable) bit in CR0 (Control Register 0)
    mov cr0, eax

    ; Perform far jump to selector 08h (offset into GDT, pointing at a 32bit PM code segment descriptor) 
    ; to load CS with proper PM32 descriptor)
    jmp 0x08:GoPMode

[bits 32]
GoPMode:

    mov esp, [savesp]
    
    ret

; the following structures are for switching in and out of real mode



savcr0:
    dd 0
savesp:
    resd 1

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

; protected mode GDT (mod this later)
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