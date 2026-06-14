//
// Created by os on 6/10/26.
//

#ifndef PROJECT_BASE_PCB_HPP
#define PROJECT_BASE_PCB_HPP
#include "../lib/hw.h"
#include "scheduler.h"

class PCB {
public:
    ~PCB() { delete[] stack; }

    bool isFinished() const { return finished; }
    void setFinished(bool f) { finished = f; }
    bool isPrivileged() const { return privileged; }
    void setPrivileged(bool f) { privileged = f; }

    using Body = void (*)(void*);
    static PCB* createPCB(Body body, void* arg, uint64* stack, bool privileged);
    static void yield();

    static PCB* running;
    static void dispatch();


private:
    PCB(Body body, void* arg, uint64* stack, bool privileged) :
        body(body),
        arg(arg),
        stack(stack),
        privileged(privileged),
        context({
            body != nullptr ? (uint64)&wrapper : 0,
            stack != nullptr ? (uint64)&stack[STACK_SIZE] : 0
        }),
        finished(false) {
        if (body != nullptr) { Scheduler::put(this); }
    }


    struct Context {
        uint64 ra;
        uint64 sp;
    };

    Body body;
    void* arg;
    uint64 *stack;
    bool privileged;
    Context context;
    bool finished;

    static void wrapper();

    static void contextSwitch(Context *oldContext, Context *newContext);

    static uint64 constexpr STACK_SIZE = 1024;

};

#endif //PROJECT_BASE_PCB_HPP
