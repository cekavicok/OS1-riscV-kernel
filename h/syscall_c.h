
#ifndef SYSCALL_C_H
#define SYSCALL_C_H

#include "../lib/hw.h"

static const uint8 CODE_MEM_ALLOC = 0x01;
static const uint8 CODE_MEM_FREE = 0x02;

static const uint8 CODE_THREAD_CREATE = 0x11;
static const uint8 CODE_THREAD_EXIT = 0x12;
static const uint8 CODE_THREAD_DISPATCH = 0x13;

static const uint8 CODE_GETC = 0x41;
static const uint8 CODE_PUTC = 0x42;

void* mem_alloc(size_t size);
int mem_free(void*);

class PCB;
typedef PCB* thread_t;
int thread_create(thread_t* handle, void (*start_routine)(void*), void* arg);
int thread_exit();
void thread_dispatch();


const int EOF = -1;
char getc();
void putc(char c);

#endif
