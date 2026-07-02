//
// Created by os on 6/10/26.
//

#include "../h/PCB.h"
#include "../h/riscV.h"
#include "../h/scheduler.h"
#include "../h/print.h"
#include "../h/syscall_c.h"
#include "../lib/console.h"
#include "../h/MemoryAllocator.h"

List<PCB> PCB::done;
PCB *PCB::running = nullptr;

uint64 PCB::timeSliceCounter = 0;


void* PCB::operator new(size_t n) {
    return MemoryAllocator::mem_alloc_alajn(n);
}

void PCB::operator delete(void* p) noexcept {
    MemoryAllocator::mem_free(p);
}

PCB::~PCB() {
    if (stack != nullptr) MemoryAllocator::mem_free(stack);
}

PCB* PCB::createPCB(Body body, void* arg, uint64* stack, bool privileged) {
    return new PCB(body, arg, stack, privileged);
}

PCB *PCB::createIdle(Body body) {
    uint64* stack = (uint64*) MemoryAllocator::mem_alloc_alajn(DEFAULT_STACK_SIZE);
    PCB* p = new PCB(body, nullptr, stack, true);
    Scheduler::pulIdle(p);
    p->setIdle(true);
    return p;
}

void PCB::yield() {
    __asm__ volatile("li a0, 0x13");
    __asm__ volatile("ecall");
    //thread_dispatch();
}

void PCB::wrapper() {
    riscV::setSppSpie(&PCB::userBodyWrapper);
}

void PCB::userBodyWrapper() {
    PCB::running->body(PCB::running->arg);
    thread_exit();
}

void PCB::exit() {
    running->setFinished(true);
    done.addLast(running);
    dispatch();
}

void PCB::clean_done() {
    unsigned long n = done.getSize();
    for (unsigned long i = 0; i < n; i++) {
        PCB* p = done.removeFirst();
        if (p == running) {
            done.addLast(p);
            continue;
        }
        delete p;
    }
}

void PCB::dispatch() {
    clean_done();
    PCB* old = running;
    if (!old->isFinished() && !old->isBlocked() && !old->isIdle()) {
        Scheduler::put(old);
    }
    running = Scheduler::get();
    PCB::contextSwitch(&old->context, &running->context);
}