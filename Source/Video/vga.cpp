#include "vga.h"
#include "../Util.cpp"
#include "../Ascii.cpp"

short column = 0;
short line   = 0;
short tabLen;
short current_cursor;

short* const vga = (short* const) 0xB8000;

void update_cursor(int x, int y)
{
	short pos = y * width + x;
    current_cursor = pos;
    

	outb(0x3D4, 0x0F);
	outb(0x3D5, (char) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (char) ((pos >> 8) & 0xFF));
}

void update_cursor_addr(short pos)
{
    current_cursor = pos; // this is for backspace operations. Divison procudes a clean integer
    column = pos%width;
    line = pos/width;

	outb(0x3D4, 0x0F);
	outb(0x3D5, (char) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (char) ((pos >> 8) & 0xFF));
}

void disable_cursor() {
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);
}

void enable_cursor()
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, (inb(0x3D5) & 0xC0) | 0);

	outb(0x3D4, 0x0B);
	outb(0x3D5, (inb(0x3D5) & 0xE0) | 25);
}



void clear_screen() {
    for (int i = 0; i < width * height; ++i) {
        vga[i] = ' ';
    }
    line = 0;
    column = 0;
    update_cursor(0,0);
}

void putchar(char character, bool bright = true, char color = 0xf) {
    if (column == width){
        line++;
        column = 0;
    }

    vga[line * width + (column)] = character | (color) << 8;
    column++;
    update_cursor(column,line);
}

void set_char_at_video_memory(char character, short addr, bool bright = true, char color = 0xf) {
    vga[addr] = character | (color | (bright << 3)) << 8;
}

short get_cursor() {
    return current_cursor;
}

void newline() {
    line++;
    column = 0;
    update_cursor(column,line);
}

void print(const char* str, bool bright = true, char color = 0xf) {
    // Print each character of the string
    
    while (*str) {
        switch(*str){
            case '\n':
                line++;
                column = 0;
                break;
            case '\r':
                column = 0;
                break;
            case '\t':
                if (column == width){
                    line++;
                    column = 0;
                }
                tabLen = 4 - (column % 4);
                while (tabLen != 0){
                    vga[line * width + (column++)] = ' ' | 0;
                    tabLen--;
                }
                break;
            default:
                if (column == width){
                    line++;
                    column = 0;
                }

                vga[line * width + (column)] = *str | (color) << 8;
                column++;
                break;

        }
        ++str;
    }
    if (line > height) {
        memory_copy((int *)0xb8000,(int *)0xb8000-(width),width*(height-1)*2);
        line = height-1;
    }
    update_cursor(column,line);
}

void printd(signed long num, bool bright = true, char color = 0xf) {
    char str[32]; // allocated 32-byte string (probably not all used anyways, I want malloc)
    int64_BCD(num, str);
    print(str, bright, color);
}

void printh(signed long num, bool bright = true, char color = 0xf) {
    char str[32];
    int64_HEX(num, str);
    print(str, bright, color);
}