#include "../lib/console.h"
#include "../h/riscV.h"
#include "../h/MemoryAllocator.h"
#include "../h/syscall_cpp.h"
#include "../h/PCB.h"
#include "../h/workers.h"
#include "../h/print.h"
//clean->qemu


void main(){
    riscV::w_stvec((uint64) &riscV::trapRoutine);
    PCB *threads[3];

    threads[0] = PCB::createPCB(nullptr);
    PCB::running = threads[0];

    threads[1] = PCB::createPCB(workerBodyA);
    printStr("Thread worker A created");


    threads[2] = PCB::createPCB(workerBodyB);
    printStr("Thread worker B created");
    while (!(threads[1]->isFinished() && threads[2]->isFinished())  ) {
        PCB::yield();
    }


    for (auto &thread: threads) {
        delete thread;
    }

    printStr("finished\n");
}