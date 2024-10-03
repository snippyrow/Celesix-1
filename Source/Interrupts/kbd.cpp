#include "kbd.h"

char kbd_buff[256];
bool upper = false;
bool kbd_enabled = false;
bool input_received = false;

int (*enter_handler)();

void (*kbd_hook)();

extern "C" void kbd_handler() {
    kbd_hook();
}



void keypress() {
    uint8_t scancode = inb(0x60); // read PS/2 scancode

    if (!kbd_enabled) {
        outb(0x20, 0x20); // EOI
        return;
    }
    
    if (scancode == 0x2a || scancode == 0x36) {
        upper = true;
    } else if (scancode == 0xaa || scancode == 0xb6) {
        upper = false;
    } else if (scancode == 0xe) {
        if (string_length(kbd_buff) > 0) { // so that you cannot exit the text buffer
            backspace(kbd_buff);
            short newCursor = get_cursor() - 1;
            set_char_at_video_memory(' ', newCursor);
            update_cursor_addr(newCursor);
        }
    } else if (scancode == 0x1c) {
        // enter key pressed
        enter_handler();
    } else if (scancode < 58) {
        if (string_length(kbd_buff) < 256) {
            char letter;
            if (upper) {
                letter = keymap_shift[(int) scancode];
            } else {
                letter = keymap[(int) scancode];
            }
            append(kbd_buff,letter);
            char str[2] = {letter, '\0'};
            print(str);
        }
    };
    input_received = true;
    outb(0x20, 0x20); // EOI
}