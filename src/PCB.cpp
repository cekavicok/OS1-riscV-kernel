//
// Created by os on 6/10/26.
//

#include "../h/PCB.h"
#include "../h/riscV.h"
#include "../h/scheduler.h"
#include "../h/print.h"
#include "../h/syscall_c.h"
#include "../lib/console.h"

PCB* g_running_shadow = nullptr;
PCB *PCB::running = nullptr;

uint64 PCB::timeSliceCounter = 0;

PCB* PCB::createPCB(Body body, void* arg, uint64* stack, bool privileged) {
    return new PCB(body, arg, stack, privileged);
}

void PCB::yield() {
    __asm__ volatile("li a0, 13");
    __asm__ volatile("ecall");
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
    if (!old->isFinished() && !old->isBlocked()) {
        Scheduler::put(old);
    }

    running = Scheduler::get();

    if (running == nullptr) {
        const char* m = "DISPATCH NULL\n";
        for (int i = 0; m[i]; i++) __putc(m[i]);
        riscV::shutdown();
    }


    PCB::contextSwitch(&old->context, &running->context);
}