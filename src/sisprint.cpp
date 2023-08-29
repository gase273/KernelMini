//
// Created by os on 6/29/22.
//

#include "../h/sisprint.h"
#include "../h/Console.h"

extern "C" void printSisStr(char const* str) {
    uint32 i = 0;
    while(str[i] != '\0') {
        KConsole::putc(str[i++]);
    }
}

extern "C" void printSisInt64(long integer64) {
    char buffer[21]; //2^64 - 1  is a 20 digit number
    short i = 0;
    int neg = 0;
    uint64 toPrint;
    if (integer64 < 0) {
        neg = 1;
        toPrint = -integer64;
    }
    else toPrint = integer64;

    do{
        buffer[i++] = digits[toPrint % 10];
    } while((toPrint /= 10) != 0);
    if (neg == 1) buffer[i++] = '-';
    while (--i >= 0) {
        KConsole::putc(buffer[i]);
    }
}

extern "C" void printSisUInt64(uint64 uintiger64) {
    char buffer[21]; //2^64 - 1  is a 20 digit number
    short i = 0;
    do{
        buffer[i++] = digits[uintiger64% 10];
    } while((uintiger64 /= 10) != 0);
    while (--i >= 0) {
        KConsole::putc(buffer[i]);
    }
}

extern "C" void printSisInt32(int integer32) {
    char buffer[11]; //2^64 - 1  is a 20 digit number
    short i = 0;
    int neg = 0;
    uint64 toPrint;
    if (integer32 < 0) {
        neg = 1;
        toPrint = -integer32;
    }
    else toPrint = integer32;

    do{
        buffer[i++] = digits[toPrint % 10];
    } while((toPrint /= 10) != 0);
    if (neg == 1) buffer[i++] = '-';
    while (--i >= 0) {
        KConsole::putc(buffer[i]);
    }
}