//
// Created by os on 6/23/26.
//

#ifndef PROJECT_BASE_KBUFFER_H
#define PROJECT_BASE_KBUFFER_H
#include "../lib/hw.h"
#include "Ksemaphore.h"

class Kbuffer {

public:
    void* operator new(size_t n);
    void operator delete(void* p) noexcept;
     explicit Kbuffer(int size);
    ~Kbuffer();

    void put(char c);
    char get();
    bool try_put(char c);

private:
    //mozda data da bude int umesto char?
    char* data;
    int size, head, tail;
    Ksemaphore* spaceAvailable;
    Ksemaphore* itemAvailable;
    Ksemaphore* mutex;
};

#endif //PROJECT_BASE_KBUFFER_H
