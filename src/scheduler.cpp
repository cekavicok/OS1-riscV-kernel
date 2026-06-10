//
// Created by os on 6/10/26.
//

#include "../h/scheduler.h"

List<PCB> Scheduler::readyQueue;

PCB *Scheduler::get() {
    return readyQueue.removeFirst();
}

void Scheduler::put(PCB *pcb) {
    readyQueue.addLast(pcb);
}
