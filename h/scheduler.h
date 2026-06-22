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
    static List<PCB> sleepQueue;

public:

    static PCB *get();
    static void put(PCB *pcb);

    static int sleep(uint64 ticks);
    static void tick();
};

#endif //PROJECT_BASE_SCHEDULER_H
