//
// Created by os on 6/10/26.
//
#include "../h/print.h"
#include "../h/syscall_cpp.h"
#include "../h/riscV.h"


void printStr(char const *str) {
    uint64 sstatus = riscV::r_sstatus();
    riscV::mask_clear_sstatus(riscV::SSTATUS_SIE);
    while (*str) {

        putc(*str++);
    }
    riscV::mask_set_sstatus( sstatus & riscV::SSTATUS_SIE ? riscV::SSTATUS_SIE : 0);
}

void printInt(uint64 integer) {
    uint64 sstatus = riscV::r_sstatus();
    riscV::mask_clear_sstatus(riscV::SSTATUS_SIE);
    static char digits[] = "0123456789";
    char buf[20];   // uint64 max je 20 cifara
    int i = 0;

    do {
        buf[i++] = digits[integer % 10];
    } while ((integer /= 10) != 0);

    while (--i >= 0) {
        putc(buf[i]);
    }
    riscV::mask_set_sstatus( sstatus & riscV::SSTATUS_SIE ? riscV::SSTATUS_SIE : 0);
}