#include "../lib/console.h"
#include "../h/riscV.h"
#include "../h/MemoryAllocator.h"
#include "../h/syscall_cpp.h"
#include "../h/Kconsole.h"
#include "../h/PCB.h"
#include "../h/workers.h"
#include "../h/print.h"
#include "../h/semaphoreTest.h"
//clean->qemu

extern void userMain();

void usermainWrapper(void* thr) {

    userMain();

}
static volatile uint64 idleCount = 0;
void idleBody(void*) {
    while (true) {}
}

static PCB* threads[2];
static PCB* idle;


int main(){


    riscV::w_stvec((uint64) &riscV::trapRoutine);

    thread_create(&threads[0], nullptr, nullptr);
    PCB::running = threads[0];
    Kconsole::init();

    idle = PCB::createIdle(idleBody);
    Scheduler::setIdle(idle);



    riscV::mask_set_sstatus(riscV::SSTATUS_SIE);

    thread_create(&threads[1], usermainWrapper, nullptr);

    while (!(threads[1]->isFinished())) {
        thread_dispatch();
    }

    for (auto &thread : threads) {
        delete thread;
    }

    riscV::shutdown();
    return 0;
}



