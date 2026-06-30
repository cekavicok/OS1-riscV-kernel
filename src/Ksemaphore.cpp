//
// Created by os on 6/16/26.
//

#include "../h/Ksemaphore.h"
#include "../h/MemoryAllocator.h"
#include "../h/riscV.h"



void* Ksemaphore::operator new(size_t n) {
    return MemoryAllocator::mem_alloc_alajn(n);
}

void Ksemaphore::operator delete(void* p) noexcept {
    MemoryAllocator::mem_free(p);
}

void* Ksemaphore::Waiter::operator new(size_t n) {
    return MemoryAllocator::mem_alloc_alajn(n);
}
void Ksemaphore::Waiter::operator delete(void* p) noexcept {
    MemoryAllocator::mem_free(p);
}

Ksemaphore* Ksemaphore::createSemaphore(unsigned init) {
    return new Ksemaphore(init);
}
Ksemaphore::~Ksemaphore() {
    close();
}

//dodato maskiranje i demaskiranje prekida

void Ksemaphore::block(uint64 n, uint64 savedSstatus) {
    Waiter* w = new Waiter(PCB::running, n);
    blockedQueue.addLast(w);
    PCB::running->setBlocked(true);
    riscV::w_sstatus(savedSstatus);
    PCB::dispatch();
}

void Ksemaphore::unblock() {
    Waiter* w = blockedQueue.removeFirst();
    w->thread->setSemRetVal(0);
    w->thread->setBlocked(false);
    Scheduler::put(w->thread);
    delete w;
}

int Ksemaphore::wait(uint64 n) {
    uint64 sstatus = riscV::r_sstatus();
    riscV::mask_clear_sstatus(riscV::SSTATUS_SIE);
    if (closed) {
        PCB::running->setSemRetVal(-1);
        riscV::w_sstatus(sstatus);
        return -1;
    }
    if (value >= (int)n) {
        value = value - (int)n;
        PCB::running->setSemRetVal(0);
        riscV::w_sstatus(sstatus);
    }
    else{
        block(n, sstatus);
    }
    return PCB::running->getSemRetVal();
}

int Ksemaphore::signal(uint64 n) {
    uint64 sstatus = riscV::r_sstatus();
    riscV::mask_clear_sstatus(riscV::SSTATUS_SIE);
    if (closed) return -1;
    value = value + (int)n;
    while (!blockedQueue.isEmpty()) {
        Waiter* w = blockedQueue.peekFirst();
        if (value >= (int)w->n) {
            value = value - (int)w->n;
            unblock();
        } else {
            break;
        }
    }
    riscV::w_sstatus(sstatus);
    return 0;
}

int Ksemaphore::close() {
    uint64 sstatus = riscV::r_sstatus();
    riscV::mask_clear_sstatus(riscV::SSTATUS_SIE);
    closed = true;
    while (!blockedQueue.isEmpty()) {
        Waiter* w = blockedQueue.removeFirst();
        w->thread->setSemRetVal(-1);
        w->thread->setBlocked(false);
        Scheduler::put(w->thread);
        delete w;
    }
    riscV::w_sstatus(sstatus);
    return 0;
}





