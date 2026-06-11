#include "../lib/console.h"
#include "../h/riscV.h"
#include "../h/MemoryAllocator.h"
#include "../h/syscall_cpp.h"
#include "../h/PCB.h"
#include "../h/workers.h"
#include "../h/print.h"
//clean->qemu


void main(){
    riscV::w_stvec((uint64)&riscV::trapRoutine);
    PCB *threads[3];

    threads[0] = PCB::createPCB(nullptr);
    PCB::running = threads[0];
    printStr("main PCB addr=");
    printInt((uint64)threads[0]);
    printStr("\n");
    threads[1] = PCB::createPCB(workerBodyA);
    printStr("A PCB addr=");
    printInt((uint64)threads[1]);
    printStr("\n");
    threads[2] = PCB::createPCB(workerBodyB);
    printStr("B PCB addr=");
    printInt((uint64)threads[2]);
    printStr("\n");
    while (!(threads[1]->isFinished() && threads[2]->isFinished())  ) {
        //printStr("Uradjen yield");
        PCB::yield();
    }


    for (auto &thread: threads) {
        delete thread;
    }

    printStr("finished\n");
}