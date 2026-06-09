//
// Created by os on 6/8/26.
//

#ifndef PROJECT_BASE_MEMORYALLOCATOR_H
#define PROJECT_BASE_MEMORYALLOCATOR_H
#include "../lib/hw.h"

class MemoryAllocator {
private:
    static void init();
public:
    struct FreeMem {
        FreeMem* next;
        FreeMem* prev;
        size_t size;
        //velicina u bajtovima, poravnata na blokove, bez header, header je velicine sizeof(FreeMem)
    };
    static FreeMem* head;
    MemoryAllocator() = default;
    ~MemoryAllocator() = default;
    static void* mem_alloc(size_t size);
    static int mem_free(void*);
};



#endif //PROJECT_BASE_MEMORYALLOCATOR_H
