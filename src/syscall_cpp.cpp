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

Thread::Thread(void (*body)(void*), void* arg) : myHandle(nullptr), body(body), arg(arg) {}
Thread::Thread() : myHandle(nullptr), body(threadWrapper), arg(this) {}
Thread::~Thread() {
    delete myHandle;
}

int Thread::start() {
    return thread_create(&myHandle, body, arg);
}

void Thread::dispatch() {
    thread_dispatch();
}

void Thread::threadWrapper(void* thread) {
    ((Thread*)thread)->run();
}


char Console::getc() {
    return ::getc();
}

void Console::putc(char c) {
    ::putc(c);
}

