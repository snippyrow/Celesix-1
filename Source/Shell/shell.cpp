#include "shell.h"

void help() {
    newline();
    for (int c=0;c<sizeof(commands) / sizeof(commands[0]);c++) {
        print(commands[c][0]);
        int len = string_length(commands[c][0]);
        for (int x=0;x<help_buffer-len;x++) {
            print(" ");
        }
        print(commands[c][1]);
        newline();
    }
    newline();
}

extern "C" void kmain(); // re-define multiboot location
char arglist[16][128];
int words;
int totalargs;

void shell_exit() {
    print("exiting..");
    asm volatile("cli");
    kmain();
}

void version() {
    print("\nCelesix Version 1.1.0\nThis version is developed for x86_64 in protected mode.\nShowcase model.\nDeveloped by SR.\n\n");
}

void getArgs() {
    char tmp[128];
    int i = 0;
    int tb = 0;
    totalargs = 0;
    while (kbd_buff[i] != '\0') {
        tmp[tb] = kbd_buff[i];
        tb++;
        if (kbd_buff[i] == ' ') {
            for (int ie=0;ie<tb;ie++) {
                arglist[totalargs][ie] = tmp[ie];
            }
            totalargs++;
            tb = 0;
        }
        i++;
    }

    // run loop once more for the last argument cut off
    for (int ie=0;ie<tb;ie++) {
        arglist[totalargs][ie] = tmp[ie];
    }
    totalargs++;

    words = totalargs;
}

void testcmd() {
    getArgs();
    newline();
    for (int l=0;l<totalargs;l++) {
        print("Arg#");
        printd(l);
        print(": ");
        print(arglist[l]);
        newline();
    }
    newline();
}

int currentATA = 0xE0;
void setdrive() {
    getArgs();
    newline();
    switch (arglist[1][1]) {
        case '0':
            currentATA = 0xE0;
            break;
        case '1':
            currentATA = 0xE1;
            break;
        case '2':
            currentATA = 0xE2;
            break;
        case '3':
            currentATA = 0xE3;
            break;
        default:
            print("No suitable drive found.\n");
    }
    print("Bootable ATA drive set to 0x");
    printh(currentATA);
    newline();
}

void tetris() {
    print("\nYou REALLY thought I programmed tetris into this thing? Well, you better wait a week or two for THAT to happen! :-)\n\n");
}


 // function pointers MUST be aligned with the help order located in shell.h
FuncPointer shellscripts[] = {
    testcmd, clear_screen, shell_exit, help, tetris, setdrive, version
};

int shell_common_stub() {
    string_upper(kbd_buff); // so that commands are not case-sensetive
    bool found = false;
    char tmpcmd[128];
    int i=0;
    while ((kbd_buff[i] != ' ') && (kbd_buff[i] != '\0')) {
        tmpcmd[i] = kbd_buff[i];
        i++;
    }
    tmpcmd[i] = '\0';

    for (int c=0;c<sizeof(commands) / sizeof(commands[0]);c++) {
        if (string_cmp(tmpcmd,commands[c][0])) {
            found = true;
            shellscripts[c]();
            break;
        }
    }

    if (!found) {
        string_lower(tmpcmd);
        print("\n[cmd]: ");
        print(tmpcmd);
        print(": command invalid\n");
    }

    print("[root ~]: ");
    clear(kbd_buff);
    return 0;
}

void start_bootshell() {
    clear_screen();
    print("Celesix [Version 1.1.2] (32)\n(c) Snippyrow. No rights reserved!\n\nType 'help' for a list of commands.\n\n"); // title card
    asm volatile ("sti");
    kbd_hook = keypress;
    enter_handler = shell_common_stub;
    kbd_enabled = true;
    isr_install(); // refreash the keyboard handler
    
    print("[root ~]: ");
    enable_cursor();
    while (1) {
        // looped so code does not go beyond this function. REALLY has a bad time.
        while (!input_received) {
            // wait for key
        }

        input_received = false;
        
    }

}