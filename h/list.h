//
// Created by os on 6/10/26.
//

#ifndef PROJECT_BASE_LIST_H
#define PROJECT_BASE_LIST_H

#include "../lib/hw.h"
#include "MemoryAllocator.h"

template<typename T>
class List {
private:
    struct Elem {
        T* data;
        Elem* next;
        Elem(T* dat, Elem* nxt) : data(dat), next(nxt) {}
    };
    Elem* head;
    Elem* tail;
    unsigned long size;

    static Elem* allocElem(T* data, Elem* next) {
        void* mem = MemoryAllocator::mem_alloc_alajn(sizeof(Elem));
        if (!mem) return nullptr;
        Elem* elem = (Elem*)mem;
        elem->data = data;
        elem->next = next;
        return elem;
    }

    static void freeElem(Elem* elem) {
        MemoryAllocator::mem_free(elem);
    }

public:
    List() : head(0), tail(0), size(0) {}
    List(const List<T>&) = delete;
    List<T>& operator=(const List<T>&) = delete;

    unsigned long getSize() const { return size; }

    bool isEmpty() const { return head == nullptr; }

    void addFirst(T* data) {
        Elem* elem = allocElem(data, head);
        if (!elem) return;
        head = elem;
        if (!tail)
            tail = head;
        ++size;
    }

    void addLast(T* data) {
        Elem* elem = allocElem(data, 0);
        if (!elem) return;
        if (tail) {
            tail->next = elem;
            tail = elem;
        }
        else
            head = tail = elem;
        ++size;
    }

    T* removeFirst() {
        if (!head) return 0;
        Elem* elem = head;
        head = head->next;
        if (!head)
            tail = 0;
        --size;
        T* ret = elem->data;
        freeElem(elem);
        return ret;
    }

    T* peekFirst() {
        if (!head) return 0;
        return head->data;
    }

    T* removeLast() {
        if (!head) return 0;
        Elem* prev = 0;
        for (Elem* curr = head; curr && curr != tail; curr = curr->next)
            prev = curr;
        Elem* elem = tail;
        if (prev)
            prev->next = 0;
        else
            head = 0;
        tail = prev;
        --size;
        T* ret = elem->data;
        freeElem(elem);
        return ret;
    }

    T* peekLast() {
        if (!tail) return 0;
        return tail->data;
    }

    bool remove(T* data) {
        if (!head) return false;
        Elem* prev = nullptr;
        for (Elem* curr = head; curr; curr = curr->next) {
            if (curr->data == data) {
                if (prev) prev->next = curr->next;
                else head = curr->next;
                if (curr == tail) tail = prev;
                --size;
                freeElem(curr);
                return true;
            }
            prev = curr;
        }
        return false;
    }
};

#endif //PROJECT_BASE_LIST_H