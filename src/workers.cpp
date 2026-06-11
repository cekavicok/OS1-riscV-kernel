//
// Created by os on 6/10/26.
//

#include "../h/workers.h"
#include "../h/PCB.h"
#include "../lib/hw.h"
#include "../h/print.h"
#include "../h/syscall_c.h"

static uint64 fibonacci(uint64 n) {
    if (n == 0 || n == 1) { return n; }
    if (n % 4 == 0) {
        PCB::yield();
    }
    return fibonacci(n - 1) + fibonacci(n - 2);
}

void workerBodyA() {
    uint8 i = 0;
    for (; i < 3; i++) {
        printStr("A: i=");
        printInt(i);
        printStr("\n");
    }

    printStr("A: yield\n");
    __asm__ ("li t1, 7");
    PCB::yield();

    uint64 t1 = 0;
    __asm__ ("mv %[t1], t1" : [t1] "=r"(t1));

    printStr("A: t1=");
    printInt(t1);
    printStr("\n");

    uint64 result = fibonacci(20);
    printStr("A: fibonacci=");
    printInt(result);
    printStr("\n");

    for (; i < 6; i++) {
        printStr("A: i=");
        printInt(i);
        printStr("\n");
    }

    PCB::running->setFinished(true);
    printStr("A: postavljen finished, running=");
    printInt((uint64)PCB::running);
    printStr("\n");
    PCB::yield();
    printStr("A: posle finalnog yield - ovo ne bi smelo da se ispiše\n");
}

void workerBodyB() {
    uint8 i = 10;
    for (; i < 13; i++) {
        printStr("B: i=");
        printInt(i);
        printStr("\n");
    }

    printStr("B: yield\n");
    __asm__ ("li t1, 5");
    PCB::yield();
    printStr("B: nastavljam posle yield, ulazim u fibonacci\n");
    uint64 result = fibonacci(23);
    printStr("B: fibonacci done\n");
    printStr("B: fibonacci=");
    printInt(result);
    printStr("\n");

    for (; i < 16; i++) {
        printStr("B: i=");
        printInt(i);
        printStr("\n");
    }

    PCB::running->setFinished(true);
    PCB::yield();
}