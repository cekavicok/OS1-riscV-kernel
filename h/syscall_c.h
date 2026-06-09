
#ifndef SYSCALL_C_H
#define SYSCALL_C_H

#include "../lib/hw.h"

static const uint8 CODE_MEM_ALLOC = 0x01;
static const uint8 CODE_MEM_FREE = 0x02;

void* mem_alloc(size_t size);
int mem_free(void*);



#endif
