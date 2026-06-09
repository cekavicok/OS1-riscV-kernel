#include "../lib/console.h"
#include "../h/riscV.h"
#include "../h/MemoryAllocator.h"
#include "../h/syscall_cpp.h"
//clean->qemu

void printStr(const char* str) {
    while (*str) __putc(*str++);
}

void main(){

    riscV::w_stvec((uint64) &riscV::trapRoutine);

    void* p1 = MemoryAllocator::mem_alloc(100);
    if (p1) printStr("p1 ok\n");
    void* p2 = MemoryAllocator::mem_alloc(200);
    if (p2) printStr("p2 ok\n");

    MemoryAllocator::mem_free(p1);
    void* p3 = MemoryAllocator::mem_alloc(50);
    if (p3) printStr("p3 ok\n");
    MemoryAllocator::mem_free(p2);
    MemoryAllocator::mem_free(p3);

    int* x = new int;
    *x = 42;
    delete x;

    printStr("done\n");

}