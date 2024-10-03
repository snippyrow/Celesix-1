#include "Video/vga.cpp"
#include "Interrupts/idt.cpp"
#include "Interrupts/kbd.cpp"
#include "Shell/shell.cpp"
#include "ATA/ata.cpp"

// main system
#include "Sys/Celesix.cpp"

//#include "Interrupts/kbd.cpp"

char modes[][2][128] {
    {"Celesix Version 1","Default boot option.    \n\tLoads into the base Celesix system.    "},
    {"Start Shell","Starts a shell at root.       \n\tUseful for debugging and development.  "},
    {"Reboot System","Restarts the bootloader.    \n\tAlso, a filler spot for testing.       "}
};

int modesize = sizeof(modes)/sizeof(modes[0]);

extern "C" void core_entry();

// core start is located in Sys/Celesix.cpp

int start_vectors[] = {
    (int)core_start,(int)start_bootshell,0x1000
};

int selected = 0;


int item_padding, end_padding;
void redraw_opt() {
    line = 5;
    column = 0;
    for (int item=0;item<sizeof(modes)/sizeof(modes[0]);item++) {
        char color;
        end_padding = width-string_length(modes[item][0])-30;
        item_padding = 15;
        if (item == selected) {
            color = 0x70;
        } else {
            color = 0x07;
        }
        for (int z=0;z<item_padding;z++) {
            putchar(' ',true,0x00);
        }
        print(modes[item][0],true,color);
        for (int z1=0;z1<end_padding;z1++) {
            char ch = ' ';
            if (z1+1==end_padding & item==selected) {
                ch = '<';
            }
            putchar(ch,true,color);
        }
        newline();
    }

    line = 18;
    column = 0;
    print("Description:\n\t",true,0xf);
    print(modes[selected][1],true,0x7);
}

void jump_to_os(uint32_t start_address) {
    // Disable interrupts (if necessary, depending on the system)

    __asm__ __volatile__ ("cli");

    // Set up the stack pointer (assuming 0x90000 as stack segment, adjust as needed)


    // write code to make jmp work
    //core_entry();
    asm volatile("jmp *%0" : : "r" (start_address));
}

int resb() {
    __asm__ __volatile__("ud2");
}
void restart() {
    set_idt_gate(0,(int)resb);
    isr_install();
    int i=1/0;
}

void get_input() {
    uint8_t scancode = inb(0x60); // read PS/2 scancode

    if (!kbd_enabled) {
        outb(0x20, 0x20); // EOI
        return;
    }

    switch(scancode) {
        case 0x1:
            restart();
            break;
        case 0x48:
            if (selected-1 < 0) {
                selected = modesize-1;
            } else {
                selected--;
            }
            break;
        case 0x50:
            if (selected+1 > modesize-1) {
                selected = 0;
            } else {
                selected++;
            }
            break;
        case 0x1c:
            enter_handler();
            break;
        default:
            break;
    }

    redraw_opt();

    outb(0x20, 0x20); // EOI
}

int select_vec() {
    line=0;
    column=0;
    jump_to_os(start_vectors[selected]);
    return 1;
}

extern "C" void kmain() {
    clear_screen();
    isr_install();
    asm volatile("sti");
    
    /*
    Celesix primary bootloader. Used for debugging and loading the OS properly.
    All unused methods have been removed for size and can be found in the previous version.
    */

    /*
    What do I need in order for the celesix main version:
    First, I would get a shell working. (done)
    Next step, I need to download the rest of the OS to memory before switching to long mode.

    1. Switch to real mode
    2. Detect good memory
    3. Read the OS part drom the disk, using real mode bios interrupts
    4. Change color mode to vga
    5. Switch to protected mode, then to long mode
    6. You in!
    */



    disable_cursor();
    char title[] = "CELESIX Proprietary Bootloader Version [1.1.3]";
    char footnote[] = "\x1e/\x1f=SELECT      ENTER=CHOOSE      ESC=EXIT";
    int padding_header = (width-string_length(title))/2;
    int padding_footer = (width-string_length(footnote))/2;

    for (int i=0;i<padding_header;i++) {
        putchar(' ',true,0x1f);
    }
    print(title,true,0x1f);
    for (int i=0;i<padding_header;i++) {
        putchar(' ',true,0x1f);
    }
    print("\n\nChoose an operating system or mode of operation.\n",true,0x0f);
    print("This list may be updated regularly..",true,0x07);

    print("\n\n");

    redraw_opt();

    
    line = 24;
    column = 0;
    for (int x1=0;x1<padding_footer;x1++) {
        putchar(' ',true,0x1f);
    }
    print(footnote,true,0x1f);
    for (int x1=0;x1<padding_footer;x1++) {
        putchar(' ',true,0x1f);
    }

    //print("[root ~]: ");
    kbd_hook = get_input;
    enter_handler = select_vec;
    kbd_enabled = true;
    
    return;
}