//
// Created by os on 6/10/26.
//

#include "../h/PCB.h"
#include "../h/riscV.h"
#include "../h/scheduler.h"

PCB *PCB::running = nullptr;

PCB* PCB::createPCB(Body body) {
    return new PCB(body);
}

void PCB::yield() {
    riscV::pushRegisters();

    PCB::dispatch();

    riscV::popRegisters();
}

void PCB::dispatch() {
    PCB* old = running;
    if (!old->isFinished()) {
        Scheduler::put(old);
    }
    running = Scheduler::get();

    PCB::contextSwitch(&old->context, &running->context);
}