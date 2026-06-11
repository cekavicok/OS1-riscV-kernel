//
// Created by os on 6/9/26.
//
#include "../h/riscV.h"

#include "../h/MemoryAllocator.h"
#include "../h/syscall_c.h"
#include "../h/print.h"

static constexpr uint64 ECALL_FROM_USER_MODE = 0x8UL;
static constexpr uint64 ECALL_FROM_SUPERVISOR_MODE = 0x9UL;
static constexpr uint64 SOFTWARE_INTERRUPT = 0x8000000000000001UL;
static constexpr uint64 EXTERNAL_INTERRUPT = 0x8000000000000009UL;


//konstante za pomeranje frame pointera(fp)
static constexpr uint64 REG_SIZE    = 8;
static constexpr uint64 A0_OFFSET   = 10 * REG_SIZE;  // x10 = a0 = 80
//ostali su sada A0_OFFSET + broj_registra * REG_SIZE;


void riscV::handleTrap(uint64* frame) {
    uint64 scause = r_scause();


    /*if (scause != ECALL_FROM_USER_MODE && scause != ECALL_FROM_SUPERVISOR_MODE) {
        printStr("!!! TRAP scause=");
        printInt(scause);
        printStr(" sepc=");
        printInt(r_sepc());
        printStr(" stval=");
        printInt(r_stval());
        printStr("\n");
        while(1); // zamrzni da vidiš output
    }*/

    if (scause == ECALL_FROM_USER_MODE || scause == ECALL_FROM_SUPERVISOR_MODE) {
        uint64 volatile sepc = r_sepc() + 4;
        uint64 volatile sstatus = r_sstatus();
        uint64 code = frame[10];
        uint64 arg1 = frame[11];

        //kod na sd x8, 8*8 fd a a0 je na 10*8 (8 je su svi double word) => a0 je na 80
        //__asm__ volatile ("mv %0, fp" : "=r"(base));
        //__asm__ volatile ("ld %0, 0(%1)" : "=r"(code) : "r"(base + A0_OFFSET));
        //__asm__ volatile ("ld %0, 0(%1)" : "=r"(arg1) : "r"(base + A0_OFFSET + 1*REG_SIZE));
        uint64 ret = 0;
        switch (code) {
            case CODE_MEM_ALLOC:
                ret = (uint64)MemoryAllocator::mem_alloc(arg1);
                break;
            case CODE_MEM_FREE:
                ret = (uint64)MemoryAllocator::mem_free((void*)arg1);
                break;
        }
        //upisujem u a0 povratnu vrednost
        //__asm__ volatile("sd %0, 0(%1)" :: "r"(ret), "r"(base + A0_OFFSET) );
        frame[10] = ret;
        w_sstatus(sstatus);
        w_sepc(sepc);
    }
    else if (scause == SOFTWARE_INTERRUPT) {        // tajmer

        uint64 volatile sepc = r_sepc() + 4;
        uint64 volatile sstatus = r_sstatus();
        __asm__ volatile("csrc sip, %0" : : "r"(0x2UL));
        w_sstatus(sstatus);
        w_sepc(sepc);
    }
    else if (scause == EXTERNAL_INTERRUPT) {        // konzola
        uint64 volatile sepc = r_sepc() + 4;
        uint64 volatile sstatus = r_sstatus();
        plic_complete(plic_claim());
        w_sstatus(sstatus);
        w_sepc(sepc);
    }


}
