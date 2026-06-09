//
// Created by os on 6/9/26.
//

#ifndef PROJECT_BASE_RISCV_H
#define PROJECT_BASE_RISCV_H

#include "../lib/hw.h"

class riscV {
public:
    static uint64 r_scause();

private:
    static void handleTrap();
};

inline uint64 riscV::r_scause() {
    uint64 volatile scause;
    __asm__ volatile ("csrr %0, scause" : "=r" (scause));
    return scause;
}


#endif //PROJECT_BASE_RISCV_H
