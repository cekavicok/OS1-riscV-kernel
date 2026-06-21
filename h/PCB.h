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
    bool isBlocked() const { return blocked; }
    void setBlocked(bool f) { blocked = f; }
    int getSemRetVal() const { return semRetValue; }
    void setSemRetVal(int v) { semRetValue = v; }
    uint64 getTimeSlice() const { return timeSlice; }
    void setTimeSlice(uint64 v) { timeSlice = v; }


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
            stack != nullptr ? (uint64)((uint8*)stack + DEFAULT_STACK_SIZE) : 0
        }),
        finished(false),
        blocked(false),
        semRetValue(0),
        timeSlice(body != nullptr ? DEFAULT_TIME_SLICE : (uint64)-1)
    {
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
    bool blocked;
    int semRetValue;
    uint64 timeSlice;

    friend class riscV;

    static void wrapper();
    static void userBodyWrapper();
    static void contextSwitch(Context *oldContext, Context *newContext);

    static uint64 timeSliceCounter;

    static uint64 constexpr STACK_SIZE = 1024;
    //static uint64 constexpr TIME_SLICE = 2;

};

#endif //PROJECT_BASE_PCB_HPP
