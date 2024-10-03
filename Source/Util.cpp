void outb(short Port, char Value){ // output to selected I/O port
    asm volatile("outb %1, %0" : : "dN" (Port), "a" (Value));
}

unsigned char inb(unsigned short port) { // get input from selected I/O port
    unsigned char result;
    __asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
}


void outw(short Port, short Value){ // output to selected I/O port
    asm volatile("outw %1, %0" : : "dN" (Port), "a" (Value));
}

short inw(unsigned short port) {
    unsigned short value;
    __asm__ __volatile__ (
        "inw %1, %0"
        : "=a" (value)  // Output operand
        : "Nd" (port)   // Input operand
    );
    return value;
}

void memory_copy(int *source, int *dest, int nbytes) { // copy memory from A to B
    int i;
    for (i = 0; i < nbytes; i++) {
        *(dest + i) = *(source + i);
    }
}

int string_length(char s[]) { // find string length (null-termed)
    int i = 0;
    while (s[i] != '\0') {
        ++i;
    }
    return i;
}

void append(char s[], char n) { // append ascii to end of string
    int len = string_length(s);
    s[len] = n;
    s[len + 1] = '\0';
}

bool backspace(char buffer[]) { // reverse append
    int len = string_length(buffer);
    if (len > 0) {
        buffer[len - 1] = '\0';
        return true;
    } else {
        return false;
    }
}

void clear(char buffer[]) { // clears a string
    int i = 0;
    while (buffer[i] != '\0') {
        buffer[i] = '\0';
        ++i;
    }
}

bool string_cmp(char bufferx[], char buffery[]) {
    int i = 0;
    while (bufferx[i] != '\0' || buffery[i] != '\0') {
        if (bufferx[i] != buffery[i]) {
            return 0;
        }
        i++;
    }
    return 1;
}

void string_lower(char buffer[]) {
    int i = 0;
    while (buffer[i] != '\0') {
        if (buffer[i] > 0x40 & buffer[i] < 0x5b) { // between capital A-Z
            buffer[i] = buffer[i]+32;
        }
        i++;
    }
}

void string_upper(char buffer[]) {
    int i = 0;
    while (buffer[i] != '\0') {
        if (buffer[i] >= 'a' && buffer[i] <= 'z') { // between lowercase a-z
            buffer[i] = buffer[i] - 32; // Convert to uppercase
        }
        i++;
    }
}

int malloc(int size) {
    
}