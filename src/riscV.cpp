//
// Created by os on 6/9/26.
//
#include "../h/riscV.h"

#include "../h/MemoryAllocator.h"
#include "../h/syscall_c.h"
#include "../h/print.h"
#include "../h/PCB.h"
#include "../lib/console.h"
#include "../h/Ksemaphore.h"

static constexpr uint64 ECALL_FROM_USER_MODE = 0x8UL;
static constexpr uint64 ECALL_FROM_SUPERVISOR_MODE = 0x9UL;
static constexpr uint64 SOFTWARE_INTERRUPT = 0x8000000000000001UL; //tajmer prekid
static constexpr uint64 EXTERNAL_INTERRUPT = 0x8000000000000009UL; //spoljasnji prekid
static constexpr uint64 ILLEGAL_INSTRUCTION = 0x2UL; //supervisor instr iz user moda
static constexpr uint64 ILLEGAL_READ = 0x5UL; //adresa citanja
static constexpr uint64 ILLEGAL_WRITE = 0x7UL; //adresa upisa




//konstante za pomeranje frame pointera(fp)
static constexpr uint64 REG_SIZE    = 8;
static constexpr uint64 A0_OFFSET   = 10 * REG_SIZE;  // x10 = a0 = 80
//ostali su sada A0_OFFSET + broj_registra * REG_SIZE;


void riscV::setSppSpie(void (*entry)()) {
    if (PCB::running->isPrivileged()) {
        mask_set_sstatus(SSTATUS_SPP);
    }
    else {
        mask_clear_sstatus(SSTATUS_SPP);
    }
    mask_set_sstatus(SSTATUS_SPIE);
    w_sepc((uint64)entry);
    __asm__ volatile ("sret");
}


void riscV::handleTrap(uint64* frame) {
    uint64 scause = r_scause();


    /*if (scause != ECALL_FROM_USER_MODE && scause != ECALL_FROM_SUPERVISOR_MODE
        && scause != SOFTWARE_INTERRUPT && scause != EXTERNAL_INTERRUPT) {
        printStr("!!! TRAP scause=");
        printInt(scause);
        printStr(" sepc=");
        printInt(r_sepc());
        printStr(" stval=");
        printInt(r_stval());
        printStr("\n");
        while (1);
        }*/

    if (scause == ECALL_FROM_USER_MODE || scause == ECALL_FROM_SUPERVISOR_MODE) {
        uint64 volatile sepc = r_sepc() + 4;
        uint64 volatile sstatus = r_sstatus();
        uint64 code = frame[10];
        uint64 arg1 = frame[11];
        uint64 arg2 = frame[12];
        uint64 arg3 = frame[13];
        uint64 arg4 = frame[14];

        //kod na sd x8, 8*8 fd a a0 je na 10*8 (8 je su svi double word) => a0 je na 80
        //__asm__ volatile ("mv %0, fp" : "=r"(base));
        //__asm__ volatile ("ld %0, 0(%1)" : "=r"(code) : "r"(base + A0_OFFSET));
        //__asm__ volatile ("ld %0, 0(%1)" : "=r"(arg1) : "r"(base + A0_OFFSET + 1*REG_SIZE));
        uint64 ret = 0;
        switch (code) {
            case CODE_MEM_ALLOC: {
                ret = (uint64)MemoryAllocator::mem_alloc(arg1);
                break;
            }
            case CODE_MEM_FREE: {
                ret = (uint64)MemoryAllocator::mem_free((void*)arg1);
                break;
            }
            case CODE_THREAD_CREATE: {
                uint64* stack = (uint64*)arg1;
                void* arg = (void*)arg2;
                PCB::Body routine = (PCB::Body)arg3;
                thread_t* handle = (thread_t*)arg4;
                bool privileged = (routine == nullptr);
                *handle = PCB::createPCB(routine, arg, stack, privileged);
                ret = *handle != nullptr ? 0 : -1;
                break;
            }
            case CODE_THREAD_EXIT: {
                PCB::running->setFinished(true);
                PCB::dispatch();
                break;
            }
            case CODE_THREAD_DISPATCH: {
                PCB::dispatch();
                break;
            }
            case CODE_SEM_OPEN: {
                sem_t* handle = (sem_t*)arg2;
                unsigned init = (unsigned)arg1;
                *handle = Ksemaphore::createSemaphore(init);
                ret = *handle != nullptr ? 0 : -1;
                break;
            }
            case CODE_SEM_CLOSE: {
                Ksemaphore* handle = (Ksemaphore*)arg1;
                ret = handle->close();
                break;
            }
            case CODE_SEM_WAIT: {
                Ksemaphore* handle = (Ksemaphore*)arg1;
                handle->wait(1);
                ret = PCB::running->getSemRetVal();
                break;
            }
            case CODE_SEM_SIGNAL: {

                break;
            }
            case CODE_SEM_WAIT_N: {
                break;
            }
            case CODE_SEM_SIGNAL_N: {
                break;
            }



            case CODE_GETC : {
                ret = (uint64)__getc();
                break;
            }
            case CODE_PUTC : {
                __putc((char)arg1);
                ret = 0;
                break;
            }
        }
        //upisujem u a0 povratnu vrednost
        //__asm__ volatile("sd %0, 0(%1)" :: "r"(ret), "r"(base + A0_OFFSET) );
        frame[10] = ret;
        w_sstatus(sstatus);
        w_sepc(sepc);
    }
    else if (scause == SOFTWARE_INTERRUPT) {        // tajmer

        uint64 volatile sepc = r_sepc();
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
    else if (scause == ILLEGAL_INSTRUCTION) {
        printStr("radi ins\n");
        shutdown();
    }
    else if (scause == ILLEGAL_READ) {
        printStr("radi read\n");
        shutdown();
    }
    else if (scause == ILLEGAL_WRITE) {
        printStr("radi write\n");
        shutdown();
    }


}
