#include "../h/syscall_cpp.h"
#include "../lib/console.h"

void* operator new(uint64 size) {
    return mem_alloc(size);
}

void* operator new[](uint64 size) {
    return mem_alloc(size);
}

void operator delete(void* ptr) {
    mem_free(ptr);
}

void operator delete[](void* ptr) {
    mem_free(ptr);
}