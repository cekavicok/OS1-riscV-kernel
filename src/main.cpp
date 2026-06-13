#include "../lib/console.h"
#include "../h/riscV.h"
#include "../h/MemoryAllocator.h"
#include "../h/syscall_cpp.h"
#include "../h/PCB.h"
#include "../h/workers.h"
#include "../h/print.h"
//clean->qemu

extern void userMain();

void usermainWrapper(void* thr) {
    userMain();
}

int main(){
    riscV::w_stvec((uint64) &riscV::trapRoutine);
    PCB* threads[2];

    thread_create(&threads[0], nullptr, nullptr);
    PCB::running = threads[0];

    thread_create(&threads[1], usermainWrapper, nullptr);
    while (!(threads[1]->isFinished())) {
        thread_dispatch();
    }
    return 0;
}