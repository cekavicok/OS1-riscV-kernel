//
// Created by os on 6/9/26.
//

#ifndef PROJECT_BASE_RISCV_H
#define PROJECT_BASE_RISCV_H

#include "../lib/hw.h"



class riscV {
public:
    static uint64 r_scause();

    //sepc
    static uint64 r_sepc();
    static void w_sepc(uint64 sepc);

    //stvec
    static void w_stvec(uint64 stvec);


    static void trapRoutine();
private:
    static void handleTrap();
};

inline uint64 riscV::r_scause() {
    uint64 volatile scause;
    __asm__ volatile ("csrr %0, scause" : "=r" (scause));
    return scause;
}

inline uint64 riscV::r_sepc() {
    uint64 volatile sepc;
    __asm__ volatile ("csrr %0, sepc" : "=r" (sepc));
    return sepc;
}

inline void riscV::w_sepc(uint64 sepc) {
    __asm__ volatile ("csrw sepc, %0" : : "r" (sepc));
}

inline void riscV::w_stvec(uint64 stvec) {
    __asm__ volatile ("csrw stvec, %0" : : "r" (stvec));
}


#endif //PROJECT_BASE_RISCV_H
