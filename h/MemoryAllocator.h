//0
// Created by os on 6/20/22.
//
#include "../lib/hw.h"

#ifndef PROJECT_BASE_V1_1_MEMORYALLOCATOR_H
#define PROJECT_BASE_V1_1_MEMORYALLOCATOR_H

class MemoryAllocator {
private:
    typedef struct MemBlockDesc {
        size_t size;
        MemBlockDesc* prev;
        MemBlockDesc* next;
    } MemBlockDesc;

    static MemoryAllocator* instance;
    static MemoryAllocator* createInstance();

    MemBlockDesc* occupiedMemHead; //head of occupied memory segments list, will always exist
    MemBlockDesc* freeMemHead; //head of free memory segments list
    MemBlockDesc* occupiedMemTail; //tail of occupied memory segments list, will always exist
    MemBlockDesc* maxSegment; //start address of the highest mapped segment (occupied or free)

    void slice (MemBlockDesc*, size_t);
    void tryMerge (MemBlockDesc*);

public:
    MemoryAllocator(const MemoryAllocator&) = delete;
    MemoryAllocator(MemoryAllocator&&) = delete;
    MemoryAllocator() = delete;
    MemoryAllocator& operator = (const MemoryAllocator&) = delete;
    MemoryAllocator& operator =  (MemoryAllocator&&) = delete;

    void* kmem_alloc(size_t);
    int kmem_free(void*);
    static MemoryAllocator* getInstance();

    const static size_t MemBlockDescSize = sizeof(MemBlockDesc);
};



#endif //PROJECT_BASE_V1_1_MEMORYALLOCATOR_H
