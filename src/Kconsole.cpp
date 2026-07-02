//
// Created by os on 6/23/26.
//

#include "../h/Kconsole.h"
#include "../h/KBuffer.h"
#include "../h/PCB.h"
#include "../h/MemoryAllocator.h"
#include "../h/riscV.h"

Kbuffer* Kconsole::outputBuffer = nullptr;
Kbuffer* Kconsole::inputBuffer = nullptr;

static const int OUTPUT_SIZE = 64;
static const int INPUT_SIZE = 64;
static const int MAX_DRAIN_PER_IRQ = 16;
volatile uint64 Kconsole::cnt = 0;

void Kconsole::init() {
    outputBuffer = new Kbuffer(OUTPUT_SIZE);
    inputBuffer = new Kbuffer(INPUT_SIZE);

    uint64* s = (uint64*) MemoryAllocator::mem_alloc_alajn(DEFAULT_STACK_SIZE);
    PCB::createPCB(writerThreadBody, nullptr, s, true);
}

bool Kconsole::txReady() {
    return (*(volatile uint8*)CONSOLE_STATUS) & CONSOLE_TX_STATUS_BIT;
}

bool Kconsole::rxReady() {
    return (*(volatile uint8*)CONSOLE_STATUS) & CONSOLE_RX_STATUS_BIT;
}

void Kconsole::rawPutc(char c) {
    while (!txReady());
    *(volatile uint8*)CONSOLE_TX_DATA = c;
}

int Kconsole::rawGetc() {
    if (!rxReady()) return -1;
    return (int)(*(volatile uint8*)CONSOLE_RX_DATA);
}

void Kconsole::writerThreadBody(void*) {
    while (true) {
        char c = outputBuffer->get();
        rawPutc(c);
        riscV::mask_clear_sstatus(riscV::SSTATUS_SIE);
        cnt = cnt - 1;
        riscV::mask_set_sstatus(riscV::SSTATUS_SIE);
    }
}

void Kconsole::putc(char c) {
    cnt = cnt + 1;
    outputBuffer->put(c);
}
char Kconsole::getc() {
    return inputBuffer->get();
}

void Kconsole::onRxAvailable() {
    int cnt = 0;
    int c;
    while (cnt < MAX_DRAIN_PER_IRQ && (c = rawGetc()) != -1) {
        inputBuffer->try_put((char)c);
        cnt++;
    }
}

void Kconsole::flush() {
    while (cnt > 0) {
        PCB::yield();
    }
    while (!txReady()) {}
}
