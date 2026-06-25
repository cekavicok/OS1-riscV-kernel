//
// Created by os on 6/10/26.
//

#include "../h/scheduler.h"
#include "../h/PCB.h"

List<PCB> Scheduler::readyQueue;
List<PCB> Scheduler::sleepQueue;

PCB *Scheduler::get() {
    return readyQueue.removeFirst();
}

void Scheduler::put(PCB *pcb) {
    readyQueue.addLast(pcb);
}

int Scheduler::sleep(uint64 ticks) {
    if (ticks == 0) return -1;
    if (PCB::running->isPrivileged()) return -1;

    PCB::running->setSleepCounter(ticks);
    PCB::running->setBlocked(true);
    sleepQueue.addLast(PCB::running);
    PCB::dispatch();
    return 0;
}

void Scheduler::tick() {
    unsigned long n = sleepQueue.getSize();
    for (unsigned long i = 0; i < n; i++) {
        PCB* p = sleepQueue.removeFirst();
        uint64 rem = p->getSleepCounter() - 1;
        if (rem == 0) {
            p->setBlocked(false);
            put(p);
        } else {
            p->setSleepCounter(rem);
            sleepQueue.addLast(p);
        }
    }
}
