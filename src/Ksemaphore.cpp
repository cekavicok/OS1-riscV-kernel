//
// Created by os on 6/16/26.
//

#include "../h/Ksemaphore.h"
#include "../h/MemoryAllocator.h"

Ksemaphore* Ksemaphore::createSemaphore(unsigned init) {
    return new Ksemaphore(init);
}
Ksemaphore::~Ksemaphore() {
    close();
}

void Ksemaphore::block(uint64 n) {
    Waiter* w = new Waiter{PCB::running, n};
    blockedQueue.addLast(w);
    PCB::running->setBlocked(true);
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
    if (closed) {
        PCB::running->setSemRetVal(-1);
        return -1;
    }
    if (value >= (int)n) {
        value = value - (int)n;
    }
    else{
        block(n);
    }
    return PCB::running->getSemRetVal();
}

int Ksemaphore::signal(uint64 n) {
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
    return 0;
}

int Ksemaphore::close() {
    closed = true;
    while (!blockedQueue.isEmpty()) {
        Waiter* w = blockedQueue.removeFirst();
        w->thread->setSemRetVal(-1);
        w->thread->setBlocked(false);
        Scheduler::put(w->thread);
        delete w;
    }
    return 0;
}

void* Ksemaphore::operator new(size_t n) {
    return MemoryAllocator::mem_alloc(n / MEM_BLOCK_SIZE + (n % MEM_BLOCK_SIZE != 0 ? 1 : 0));
}

void Ksemaphore::operator delete(void* p) noexcept {
    MemoryAllocator::mem_free(p);
}




