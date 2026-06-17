//
// Created by os on 6/17/26.
//

#include "../h/syscall_c.h"
#include "../h/semaphoreTest.h"

static void putStr(const char* s) {
    while (*s) putc(*s++);
}

static void putNum(int n) {
    if (n < 0) { putc('-'); n = -n; }
    char buf[12];
    int i = 0;
    do { buf[i++] = '0' + (n % 10); n /= 10; } while (n);
    while (i > 0) putc(buf[--i]);
}

// --- Test 1: osnovni wait/signal ---
static sem_t s1;
static int counter1 = 0;

static void basicWorkerA(void* arg) {
    sem_wait(s1);
    counter1 = 1;
    putStr("A: counter1 = "); putNum(counter1); putStr("\n");
    sem_signal(s1);
}

static void basicWorkerB(void* arg) {
    sem_wait(s1);
    counter1 = 2;
    putStr("B: counter1 = "); putNum(counter1); putStr("\n");
    sem_signal(s1);
}

// --- Test 2: wait_n / signal_n ---
static sem_t s2;

static void waitNWorker(void* arg) {
    putStr("waitNWorker: trazim 3 jedinice...\n");
    int ret = sem_wait_n(s2, 3);
    putStr("waitNWorker: dobio 3 jedinice, ret="); putNum(ret); putStr("\n");
}

static void signalNWorker(void* arg) {
    putStr("signalNWorker: spava malo (yield x3)\n");
    thread_dispatch();
    thread_dispatch();
    thread_dispatch();
    putStr("signalNWorker: signal_n(2)\n");
    sem_signal_n(s2, 2);
    putStr("signalNWorker: signal_n(1)\n");
    sem_signal_n(s2, 1);
}

// --- Test 3: sem_close dok je nit blokirana ---
static sem_t s3;

static void closeTestWorker(void* arg) {
    putStr("closeTestWorker: cekam na s3 (nikad signalisan)...\n");
    int ret = sem_wait(s3);
    putStr("closeTestWorker: probudjen, ret="); putNum(ret); putStr(" (treba -1)\n");
}

void SemaphoreTest() {
    putStr("=== TEST 1: basic wait/signal ===\n");
    sem_open(&s1, 1);
    thread_t a, b;
    thread_create(&a, basicWorkerA, nullptr);
    thread_create(&b, basicWorkerB, nullptr);
    thread_dispatch();
    thread_dispatch();
    thread_dispatch();
    sem_close(s1);
    putStr("=== TEST 1 KRAJ ===\n\n");

    putStr("=== TEST 2: wait_n / signal_n ===\n");
    sem_open(&s2, 0);
    thread_t w, sg;
    thread_create(&w, waitNWorker, nullptr);
    thread_create(&sg, signalNWorker, nullptr);
    thread_dispatch();
    for (int i = 0; i < 10; i++) thread_dispatch();
    sem_close(s2);
    putStr("=== TEST 2 KRAJ ===\n\n");

    putStr("=== TEST 3: sem_close blokirane niti ===\n");
    sem_open(&s3, 0);
    thread_t c;
    thread_create(&c, closeTestWorker, nullptr);
    thread_dispatch();
    putStr("main: zatvaram s3...\n");
    sem_close(s3);
    thread_dispatch();
    putStr("=== TEST 3 KRAJ ===\n");
}