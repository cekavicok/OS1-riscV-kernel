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

    //SPP = 0 -> userMode
    //SPP = 1 -> supervisorMode
    enum BitMaskSstatus {
        SSTATUS_SIE = (1<<1),
        SSTATUS_SPIE = (1<<5),
        SSTATUS_SPP = (1<<8),
    };

    enum BitMaskSip {
        SIP_SSIP = (1 << 1),
        SIP_STIP = (1 << 5),
        SIP_SEIP = (1 << 9)
    };

    static void mask_set_sstatus(uint64 mask);
    static void mask_clear_sstatus(uint64 mask);

    static uint64 r_sip();
    static void w_sip(uint64 sip);

    static uint64 r_sstatus();
    static void w_sstatus(uint64 sstatus);
    static void setSppSpie(void (*entry)());
    static void trapRoutine();
    static void shutdown();
private:
    static void handleTrap(uint64* frame);
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


inline uint64 riscV::r_sstatus() {
    uint64 volatile sstatus;
    __asm__ volatile ("csrr %0, sstatus" : "=r" (sstatus));
    return sstatus;
}

inline void riscV::w_sstatus(uint64 sstatus) {
    __asm__ volatile ("csrw sstatus, %0" : : "r" (sstatus));
}

inline void riscV::mask_set_sstatus(uint64 mask) {
    //csrs samo date bitove stavlja na 1 tj radi OR
    //csrc sve bitove maske koji su 1 postavi na 0, AND sa komplementom
    __asm__ volatile("csrs sstatus, %0" : : "r" (mask));
}

inline void riscV::mask_clear_sstatus(uint64 mask) {
    __asm__ volatile("csrc sstatus, %0" : : "r" (mask));
}

inline void riscV::shutdown() {
    __asm__ volatile ("li t0, 0x5555");
    __asm__ volatile ("li t1, 0x100000");
    __asm__ volatile ("sw t0, 0(t1)");
    //*(volatile uint32*)0x100000 = 0x5555;
}


#endif //PROJECT_BASE_RISCV_H
