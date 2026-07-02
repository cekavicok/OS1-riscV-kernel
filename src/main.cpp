
#include "../h/riscV.h"
#include "../h/syscall_cpp.h"
#include "../h/Kconsole.h"
#include "../h/PCB.h"

//clean->qemu

extern void userMain();

static volatile bool flag = false;

void usermainWrapper(void* thr) {
    userMain();
    flag = true;
}

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
    //mora da stoji pre pokretanja prekida
    idle = PCB::createIdle(idleBody);
    Scheduler::setIdle(idle);



    riscV::mask_set_sstatus(riscV::SSTATUS_SIE);

    thread_create(&threads[1], usermainWrapper, nullptr);

    while (!flag) {
        thread_dispatch();
    }
    Kconsole::flush();
    riscV::mask_clear_sstatus(riscV::SSTATUS_SIE);

    riscV::shutdown();
    return 0;
}



