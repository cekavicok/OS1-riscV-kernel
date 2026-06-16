//
// Created by os on 6/10/26.
//

#include "../h/PCB.h"
#include "../h/riscV.h"
#include "../h/scheduler.h"
#include "../h/print.h"
#include "../h/syscall_c.h"

PCB *PCB::running = nullptr;

PCB* PCB::createPCB(Body body, void* arg, uint64* stack, bool privileged) {
    return new PCB(body, arg, stack, privileged);
}

void PCB::yield() {
    riscV::pushRegisters();
    PCB::dispatch();
    riscV::popRegisters();
}

void PCB::wrapper() {
    riscV::setSppSpie(&PCB::userBodyWrapper);
}

void PCB::userBodyWrapper() {
    PCB::running->body(PCB::running->arg);
    thread_exit();
}

void PCB::dispatch() {
    PCB* old = running;
    //printStr("D:");printInt((uint64)old);printStr("->");
    if (!old->isFinished() && !old->isBlocked()) {
        Scheduler::put(old);
    }
    running = Scheduler::get();
    //printStr(">");printInt((uint64)running);printStr("\n");
    PCB::contextSwitch(&old->context, &running->context);
}