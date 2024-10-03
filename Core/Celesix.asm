[bits 32]
[global core_entry]

DATASEL16 equ gdt_data-gdt_start  ; Example definition of a 16-bit data selector

idt_real:
	dw 0x3ff		; 256 entries, 4b each = 1K
	dd 0			; Real Mode IVT @ 0x0000

savcr0:
	dd 0			; Storage location for pmode CR0.
savah:
    db 0
buffer:
    db '000', 0        ; Buffer for storing ASCII digits, plus null terminator
strptr:
    db 0



str_diskerror:
    db "[ ERROR ]: Reading Hard Disk #",0
str_wait:
    db "[ WARNING ]: Waiting for the primary drive to start [0x80]..",0
str_read:
    db "[ WARNING ]: Reading from disk..",0
str_test:
    db "[ WARNING ]: Read sectors, now booting..",0
str_done:
    db "[ INFO ]: Finished!",0
hexDigits:
    db "0123456789ABCDEF",0




; this is the temporary real-mode GDT. Ring 0
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

gdt2:
    dq 0x0000000000000000       ; Null descriptor
    dq 0x00CF9A000000FFFF       ; Code segment, 4GB limit, base = 0
    dq 0x00CF92000000FFFF       ; Data segment, 4GB limit, base = 0

gdt_ptr2:
    dw gdt_end2 - gdt2 - 1        ; Limit (size of GDT)
    dd gdt2                      ; Base address of GDT

gdt_end2:


core_entry:
    cli
    mov esp, 0x90000 ; reset the stack

    mov al, 'W'
    mov ah, 0xf
    mov [0xb8000], ax
    
    lgdt [gdt_descriptor] ; load temporary GDT

    jmp 0x08:Entry16
    ; switch to real mode, for using BIOS interrupts. Use long jump

[bits 16]
Disk_Read_Error:
    mov [savah], ah
    mov si, str_diskerror
    call print_string


    mov al, [savah]
    add al, 0x30
    int 0x10
    call ret_cursor

    
    jmp $
    ret
ret

Change_Cursor:  ; Row, Column (12, 35 ex.)
    mov  bh, 0       ; DisplayPage (set to 0)
    mov  ah, 2     ; BIOS.SetCursorPosition function
    int  0x10         ; Call BIOS interrupt 10h to set the cursor position
    ret

ret_cursor:
    pusha
    mov ah, 0xe
    mov  al, 0xa
    int  10h
    mov  al, 0xd
    int  10h
    
    popa
    ret

print_string:
    mov ah, 0xe
    mov al, [si]
    ploop:
        mov al, [si]
        int 0x10
        inc si
        cmp al, 0
        jne ploop

    ret
ret

%macro printf 1        ; Define a macro named 'print_char' with 1 argument
    mov si, %1
    call print_string
    call ret_cursor
%endmacro

switch_to_unreal_mode:
    cli                     ; Disable interrupts

    ; Load the GDT with extended limits
    lgdt [gdt_descriptor]

    ; Enable protected mode
    mov eax, cr0
    or eax, 1                ; Set PE bit to enable protected mode
    mov cr0, eax

    ; Load 16-bit data segment selectors
    mov ax, DATASEL16        ; Use 16-bit data selector with extended limits
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Return to real mode but keep the segment limits extended
    mov eax, cr0
    and eax, 0xFFFFFFFE      ; Clear PE bit to disable protected mode
    mov cr0, eax

    sti                     ; Enable interrupts
    ret



Entry16:

    cli
    mov eax, DATASEL16	; 16-bit Protected Mode data selector.

    mov ds, eax
	mov es, eax
	mov fs, eax
	mov gs, eax
	mov ss, eax

    ; Disable paging (we need everything to be 1:1 mapped).
	mov eax, cr0
	mov [savcr0], eax	; save pmode CR0
    and eax, 0xFFFFFFFE	; Disable paging bit & disable 16-bit pmode.
	mov cr0, eax


	jmp 0:GoRMode		; Perform Far jump to set CS.

GoRMode:
    
	mov sp, 0x8000		; pick a stack pointer.
	mov ax, 0		; Reset segment registers to 0.
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	lidt [idt_real]
	;sti			; Restore interrupts -- be careful, unhandled int's will kill it.

    mov  ax, 0003h    ; BIOS.SetVideoMode 80x25 16-color text
    int  10h

    mov ax, 0x2401
    int 0x15 ; desperate play to enable A20 line

    printf str_wait

wait_for_drive_ready:
    ; Check Drive Status
    mov ah, 0x00      ; Function: Check Drive Status
    mov dl, 0x80      ; Drive number (0x80 = first hard disk)
    int 0x13

    ; Check the status in AH
    test ah, 0x80     ; Check if the drive is ready (AH bit 7 should be clear)
    jz Read_Disk    ; Jump if zero (drive is ready)

    jmp wait_for_drive_ready


Read_Disk:
    printf str_done

    printf str_read

    ; core part of the OS that reads everything to memory.
    ; The OS is placed in the starting address of 0x01000000 (16MB)
    ; CHS (0/0/1 to 1023/255/63)

    ; find a way to write to high addresses

    ; call switch_to_unreal_mode

    

    mov ax, 0x100 ; es
    mov es, ax

    mov bx, 0x0 ; for some reason must be higher value to work?

    mov ah, 0x2    ; interrupt cmd
    mov al, 10   ; total number of sectors to read (max 54)
    mov ch, 0    ; cylinder to read (none needed for now)
    mov cl, 1    ; Starting sector (1 - 54)
    mov dh, 1    ; The specified head to read from 0-255 (0 is for boot)

    ; found! AX is overwriting ah and al

    ; for some reason when al is more than 10 I get error

    mov dl, 0x80 ; primary drive name

    int 0x13
    jc Disk_Read_Error
    

    printf str_test
    add al, 0x30
    mov ah, 0xe
    int 0x10
    call ret_cursor

    ; error? probably reading stuff that isnt there, causing an error..
    hlt
