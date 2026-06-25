//
// Created by os on 6/16/26.
//

#ifndef PROJECT_BASE_SEMAPHORE_H
#define PROJECT_BASE_SEMAPHORE_H

#include "PCB.h"
#include "list.h"


class Ksemaphore {

public:
    void* operator new(size_t n); //sprecava da se pozove u main new Ksemaphore
    void operator delete(void* p) noexcept;
    static Ksemaphore* createSemaphore(unsigned init = 1);
    ~Ksemaphore();

    int wait(uint64 n = 1);
    int signal(uint64 n = 1);
    int close();

private:
    explicit Ksemaphore(unsigned init = 1) : value((int)init), closed(false) {}

    struct Waiter {
        PCB* thread;
        uint64 n;
        Waiter(PCB* t, uint64 n): thread(t), n(n) {}
        void* operator new(size_t n);
        void operator delete(void* p) noexcept;
    };

    void block(uint64 n, uint64 savedSstatus);
    void unblock();

    int value;
    bool closed;
    List<Waiter> blockedQueue;


};


#endif //PROJECT_BASE_SEMAPHORE_H
