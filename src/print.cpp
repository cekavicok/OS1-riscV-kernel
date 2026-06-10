//
// Created by os on 6/10/26.
//
#include "../h/print.h"
#include "../lib/console.h"


void printStr(char const *str) {
    while (*str) __putc(*str++);
}

void printInt(uint64 integer) {
    static char digits[] = "0123456789";
    char buf[20];   // uint64 max je 20 cifara
    int i = 0;

    do {
        buf[i++] = digits[integer % 10];
    } while ((integer /= 10) != 0);

    while (--i >= 0) {
        __putc(buf[i]);
    }
}