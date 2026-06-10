//
// Created by os on 6/10/26.
//

#ifndef PROJECT_BASE_SCHEDULER_H
#define PROJECT_BASE_SCHEDULER_H
#include "list.h"

class PCB;

class Scheduler {
private:
    static List<PCB> readyQueue;

public:

    static PCB *get();

    static void put(PCB *pcb);
};

#endif //PROJECT_BASE_SCHEDULER_H
