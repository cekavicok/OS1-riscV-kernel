//
// Created by os on 6/8/26.
//

#include "../h/MemoryAllocator.h"

MemoryAllocator::FreeMem* MemoryAllocator::head = nullptr;


void MemoryAllocator::init() {
    static uint8 flag = false;
    //pusta da se jednom inicijalizuje
    if (flag) return;
    flag = true;
    head = (FreeMem*)HEAP_START_ADDR;
    head->next = nullptr;
    head->prev = nullptr;
    //oduzimam sizeof(FreeMem) zbog hedera
    head->size = (size_t)HEAP_END_ADDR - (size_t)HEAP_START_ADDR - sizeof(FreeMem);
}

static inline size_t alajn(size_t size) {
    size_t blocks = size/MEM_BLOCK_SIZE;
    if (size%MEM_BLOCK_SIZE) blocks++;
    return blocks*MEM_BLOCK_SIZE;
}

void* MemoryAllocator::mem_alloc(size_t size) {
    init();
    if (size == 0) return nullptr;


    //alajn mi ne treba jer syscall_c salje vec poravnatu vrednost
    size_t aligned = size*MEM_BLOCK_SIZE;



    FreeMem* curr = head;
    while (curr != nullptr && curr->size < aligned) {
        curr = curr->next;
    }
    if (curr == nullptr) return nullptr; //ako nema mesto?
    size_t rem = curr->size - aligned;
    if (rem > sizeof(FreeMem)) {
        //pravim novi blok
        //ostatak od curr->size je iza novog zauzetog size + header novog tj sizeof(FreeMem)
        FreeMem* newFreeMem = (FreeMem*)((size_t)curr+sizeof(FreeMem)+aligned);
        newFreeMem->size = rem - sizeof(FreeMem);
        newFreeMem->next = curr->next;
        newFreeMem->prev = curr->prev;
        if (curr->prev) curr->prev->next = newFreeMem;
        else head = newFreeMem;
        if (curr->next) curr->next->prev = newFreeMem;
    } else {
        if (curr->prev) curr->prev->next = curr->next;
        else head = curr->next;
        if (curr->next) curr->next->prev = curr->prev;
    }
    return (void*)((size_t)curr+sizeof(FreeMem));
}

int   MemoryAllocator::mem_free(void* start) {
    init();
    if (start == nullptr) return -1;
    FreeMem* Free = (FreeMem*)((size_t)start - sizeof(FreeMem));
    if (!head) {
        head = Free;
        Free->next = Free->prev = nullptr;
        return 0;
    }
    if ((size_t)Free<(size_t)head) {
        Free->next = head;
        Free->prev = nullptr;
        head->prev = Free;
        head= Free;
    }
    else {
        FreeMem* curr = head;
        while (curr->next && (size_t)curr->next < (size_t)Free) {
            curr = curr->next;
        }
        Free->next = curr->next;
        if (Free->next) Free->next->prev = Free;
        Free->prev = curr;
        curr->next = Free;
    }
    //gore
    if(Free->next && (size_t)Free + Free->size + sizeof(FreeMem) == (size_t)Free->next){
        Free->size += Free->next->size + sizeof(FreeMem);
        Free->next = Free->next->next;
        if(Free->next) Free->next->prev = Free;
    }
    //dole
    if(Free->prev && (size_t)Free == (size_t)Free->prev + sizeof(FreeMem) + Free->prev->size){
        Free->prev->size += Free->size + sizeof(FreeMem);
        Free->prev->next = Free->next;
        if(Free->next) Free->next->prev = Free->prev;
    }
    return 0;
}




