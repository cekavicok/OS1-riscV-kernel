//
// Created by os on 6/9/26.
//

#ifndef PROJECT_BASE_RISCV_H
#define PROJECT_BASE_RISCV_H

#include "../lib/hw.h"



class riscV {
public:

    //push x3 .. x31 na stek
    static void pushRegisters();
    //pop x3 .. x31 sa steka
    static void popRegisters();

    //scause
    static uint64 r_scause();
    static void w_scause(uint64 scause);

    //sepc
    static uint64 r_sepc();
    static void w_sepc(uint64 sepc);

    //stvec
    static uint64 r_stvec();
    static void w_stvec(uint64 stvec);

    //stval
    static uint64 r_stval();
    static void w_stval(uint64 stval);

    enum BitMaskSstatus {
        SSTATUS_SIE = (1<<1),
        SSTATUS_SPIE = (1<<5),
        SSTATUS_SPP = (1<<8),
    };

    static void trapRoutine();
private:
    static void handleTrap(uint64 frame);
};

inline uint64 riscV::r_scause() {
    uint64 volatile scause;
    __asm__ volatile ("csrr %0, scause" : "=r" (scause));
    return scause;
}

inline void riscV::w_scause(uint64 scause) {
    __asm__ volatile ("csrw scause, %0" : : "r" (scause));
}

inline uint64 riscV::r_sepc() {
    uint64 volatile sepc;
    __asm__ volatile ("csrr %0, sepc" : "=r" (sepc));
    return sepc;
}

inline void riscV::w_sepc(uint64 sepc) {
    __asm__ volatile ("csrw sepc, %0" : : "r" (sepc));
}

inline uint64 riscV::r_stvec() {
    uint64 volatile stvec;
    __asm__ volatile ("csrr %0, stvec" : "=r" (stvec));
    return stvec;
}

inline void riscV::w_stvec(uint64 stvec) {
    __asm__ volatile ("csrw stvec, %0" : : "r" (stvec));
}

inline uint64 riscV::r_stval() {
    uint64 volatile stval;
    __asm__ volatile ("csrr %0, stval" : "=r" (stval));
    return stval;
}

inline void riscV::w_stval(uint64 stval) {
    __asm__ volatile ("csrw stval, %0" : : "r" (stval));
}




#endif //PROJECT_BASE_RISCV_H
