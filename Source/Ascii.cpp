// or use malloc() if it was avalible in Util.cpp
// Takes number and string, converts number into a temporary string, then reverses it.
// Numerical reversing causes leading zeroes.
void int64_BCD(signed long num, char* str) {
    if (str == nullptr) {
        return;
    } else if (num == 0) {
        *(str) = '0';
        *(str+1) = '\0';
        return;
    } else if (num < 0) {
        *(str++) = '-';
        num = -num;
    }
    
    int i=0;
    char tmp[32];
    while (num) {
        tmp[i++] = (num % 10) + '0';
        num /= 10;
    }
    while (i+1) {
        *(str++) = tmp[i-1];
        i--;
    }
    *(str) = '\0';
    return;
}

void int64_HEX(signed long num, char* str) {
    if (str == nullptr) {
        return;
    } else if (num == 0) {
        *(str) = '0';
        *(str+1) = '\0';
        return;
    } else if (num < 0) {
        *(str++) = '-';
        num = -num;
    }
    
    int i=0;
    char code;
    char additive;
    char tmp[32];
    while (num) {
        code = num & 0xf;
        if (code > 9) {
            additive = 'a';
            code = code - 10;
        } else {
            additive = '0';
        }
        tmp[i++] = (code + additive);
        num >>= 4;
    }
    while (i+1) {
        *(str++) = tmp[i-1];
        i--;
    }
    *(str) = '\0';
    return;
}