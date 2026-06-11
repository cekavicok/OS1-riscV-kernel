//
// Created by os on 6/10/26.
//

#include "../h/PCB.h"
#include "../h/riscV.h"
#include "../h/scheduler.h"
#include "../h/print.h"

PCB *PCB::running = nullptr;

PCB* PCB::createPCB(Body body) {
    return new PCB(body);
}

void PCB::yield() {
    riscV::pushRegisters();
    PCB::dispatch();
    riscV::popRegisters();
}

void PCB::wrapper() {
    running->body();
    running->setFinished(true);
    PCB::dispatch();
}

void PCB::dispatch() {
    PCB* old = running;
    //printStr("D:");printInt((uint64)old);printStr("->");
    if (!old->isFinished()) {
        Scheduler::put(old);
    }
    running = Scheduler::get();
    //printStr(">");printInt((uint64)running);printStr("\n");

    PCB::contextSwitch(&old->context, &running->context);
}