//
// Created by os on 6/16/26.
//

#ifndef PROJECT_BASE_SEMAPHORE_H
#define PROJECT_BASE_SEMAPHORE_H

#include "PCB.h"
#include "list.h"


class Ksemaphore {
    void* operator new(size_t n); //sprecava da se pozove u main new Ksemaphore
    void operator delete(void* p) noexcept;
public:
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
    };

    void block(uint64 n);
    void unblock();

    int value;
    bool closed;
    List<Waiter> blockedQueue;


};


#endif //PROJECT_BASE_SEMAPHORE_H
