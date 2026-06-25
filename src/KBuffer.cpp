//
// Created by os on 6/23/26.
//


#include "../h/KBuffer.h"
#include "../h/MemoryAllocator.h"

static inline size_t alajn(size_t size) {
    size_t blocks = size/MEM_BLOCK_SIZE;
    if (size%MEM_BLOCK_SIZE) blocks++;
    return blocks;
}

Kbuffer::Kbuffer(int size): size(size+1), head(0), tail(0) {
    data = (char*) MemoryAllocator::mem_alloc(alajn((size_t)this->size));
    spaceAvailable = Ksemaphore::createSemaphore(size);
    itemAvailable = Ksemaphore::createSemaphore(0);
    mutex = Ksemaphore::createSemaphore(1);
}

Kbuffer::~Kbuffer() {
    MemoryAllocator::mem_free(data);
    delete spaceAvailable;
    delete itemAvailable;
    delete mutex;
}

void Kbuffer::put(char c) {
    spaceAvailable->wait();
    mutex->wait();
    data[tail] = c;
    tail = (tail+1)%size;
    mutex->signal();
    itemAvailable->signal();
}

char Kbuffer::get() {
    itemAvailable->wait();
    mutex->wait();
    char c = data[head];
    head = (head+1)%size;
    mutex->signal();
    spaceAvailable->signal();
    return c;
}

bool Kbuffer::try_put(char c) {
    int nextTail = (tail+1)%size;
    if (nextTail == head) return false;
    data[tail] = c;
    tail = nextTail;
    itemAvailable->signal();
    return true;
}

void* Kbuffer::operator new(size_t n) {
    return MemoryAllocator::mem_alloc(alajn(n));
}
void Kbuffer::operator delete(void* p) noexcept {
    MemoryAllocator::mem_free(p);
}

