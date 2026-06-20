#include "../lib/console.h"
#include "../h/riscV.h"
#include "../h/MemoryAllocator.h"
#include "../h/syscall_cpp.h"
#include "../h/PCB.h"
#include "../h/workers.h"
#include "../h/print.h"
#include "../h/semaphoreTest.h"
//clean->qemu




extern void userMain();

void usermainWrapper(void* thr) {

    userMain();

}

static PCB* threads[2];

static const char* hexDigits = "0123456789ABCDEF";
inline void printHex64(const char* label, uint64 val) {
    for (int i = 0; label[i]; i++) __putc(label[i]);
    for (int shift = 60; shift >= 0; shift -= 4) {
        __putc(hexDigits[(val >> shift) & 0xF]);
    }
    __putc('\n');
}

int main(){

    uint64 sp_val;
    __asm__ volatile ("mv %0, sp" : "=r"(sp_val));

    printHex64("SP_BOOT=0x", sp_val);
    printHex64("HEAP_START=0x", (uint64)HEAP_START_ADDR);
    printHex64("RUNNING_ADDR=0x", (uint64)&PCB::running);


    riscV::w_stvec((uint64) &riscV::trapRoutine);
    riscV::mask_set_sstatus(riscV::SSTATUS_SIE);
    thread_create(&threads[0], nullptr, nullptr);
    PCB::running = threads[0];

    //TEST ZA SEMAPHORE IZ semaphoreTest

    //SemaphoreTest();

    thread_create(&threads[1], usermainWrapper, nullptr);

    while (!(threads[1]->isFinished())) {

        thread_dispatch();

    }
    for (auto &thread : threads) {
        delete thread;
    }

    return 0;

}



