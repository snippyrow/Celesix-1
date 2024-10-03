int floor(int value) {
    int intPart = static_cast<int>(value); // Convert to int (truncates the decimal part)
    
    // Check if the number was negative and had a fractional part
    if (value < 0 && value != intPart) {
        return intPart - 1; // Round down to the next lower integer
    }
    
    return intPart;
}

int ceil(int value) {
    int intPart = static_cast<int>(value); // Convert to int (truncates the decimal part)

    // Check if the number is positive and has a fractional part
    if (value > intPart) {
        return intPart + 1; // Round up to the next higher integer
    }

    return intPart;
}

//extern "C" void cboot();
extern "C" void set_VGA();
int StartVC;

int core_start() {
    StartVC = 0x9800;
    set_VGA();
    clear_screen();
    print("test!\n");
    /*
    for (int x=0;x<32*3;x++) {
        for (int y=0;y<32*3;y++) {
            unsigned int* location = (unsigned int*)0xA0000 + 320 * (100+y) + (100+x);
            *location = 0xffffff;
        }
    }
    */
    //uint8_t buffer[512 * 63]; // Buffer for reading 63 sectors (each 512 bytes)
    uint8_t *buffer = (uint8_t *)StartVC;
    uint32_t lba = 64;         // Starting LBA address
    uint32_t sector_count = 1; // Number of sectors to read


    // must be loaded into lower memory for real mode to work
    //read_sectors(lba, buffer, sector_count);


    //__asm__ __volatile__ ("cli");
    //asm volatile("jmp *%0" : : "r" ((int)StartVC));


    // Print or use the data from the buffer
    // For example, print the first 16 bytes (for testing)

    // CUrrent issue: when using a linker to jump to cboot it works, but when jumping it does not go to real mode.
    

    /*
    print("0: ");
    for (uint32_t i = 0; i < 384; ++i) {
        printh(buffer[i]);  // Print the buffer value at index i
        //putchar(buffer[i]);
        putchar(' ');       // Print a space

        // Check if i is at a new line position (i.e., every 16 elements)
        if (i % 16 == 15) { // Print line number at the end of each line
            newline();      // Print a newline
            printh(((i+1) / 16) * 16); // Print the current segment start (no need for ceil here)
            print(": ");   // Print a colon and space
        }
    }
    */

    
    



    while (1) {}
}