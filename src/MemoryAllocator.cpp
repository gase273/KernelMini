//
// Created by os on 6/20/22.
//
#include "../h/MemoryAllocator.h"

MemoryAllocator* MemoryAllocator::instance = nullptr;

MemoryAllocator* MemoryAllocator::createInstance() {
    auto* newInstance =(MemoryAllocator*)((char*)HEAP_START_ADDR + sizeof(MemBlockDesc));
    auto* firstSegment = (MemBlockDesc*)HEAP_START_ADDR;
    firstSegment->prev = nullptr;
    firstSegment->next = nullptr;
    firstSegment->size = 1;
    newInstance->occupiedMemHead = firstSegment;
    newInstance->occupiedMemTail = firstSegment;
    newInstance->maxSegment = firstSegment;
    newInstance->freeMemHead = nullptr;
    return newInstance;
}

MemoryAllocator* MemoryAllocator::getInstance() {
    if (MemoryAllocator::instance == nullptr) {
        MemoryAllocator::instance = createInstance();
    }
    return instance;
}

void MemoryAllocator::slice(MemBlockDesc* iter, size_t size) {
    auto* newDesc = (MemBlockDesc*)((char*)iter+size*MEM_BLOCK_SIZE); //create new free segment from leftover
    if (iter == maxSegment) maxSegment = newDesc;
    newDesc->size = iter->size - size;
    newDesc->prev = iter->prev;
    if(newDesc->prev != nullptr) newDesc->prev->next = newDesc;
    else freeMemHead = newDesc;
    newDesc->next = iter->next;
    if (newDesc->next != nullptr) newDesc->next->prev = newDesc;
    iter->next = nullptr;
    iter->prev = nullptr; //remove iter from freeMemList
    iter->size = size;
}

void MemoryAllocator::tryMerge(MemBlockDesc* freeBlock) {
    if(freeBlock == nullptr) return;
    if(freeBlock->next != nullptr && (char*)freeBlock+freeBlock->size*MEM_BLOCK_SIZE == (char*)freeBlock->next) { //remove freeBlock->next segment
        if(freeBlock-> next == maxSegment) maxSegment = freeBlock;
        freeBlock->size += freeBlock->next->size;
        freeBlock->next = freeBlock->next->next;
        if (freeBlock->next != nullptr) freeBlock->next->prev = freeBlock;
    }
}

void* MemoryAllocator::kmem_alloc(size_t size) {
    for (MemBlockDesc* iter = freeMemHead; iter != nullptr; iter = iter->next) { //search in free blocks for first fit
        if (iter->size >= size) {
            if(iter->size > size) slice(iter, size); //slices the segment into 2 segments
            else {
                if(iter->next != nullptr) iter->next->prev = iter->prev;
                if(iter->prev != nullptr) iter->prev->next = iter->next;
                else freeMemHead = iter->next;
            }
            MemBlockDesc* occIter = occupiedMemHead;
            for(; occIter->next != nullptr && occIter->next < iter; occIter = occIter->next);
            iter->next = occIter->next;
            iter->prev = occIter;
            if(occIter->next != nullptr) occIter->next->prev = iter;
            else occupiedMemTail = iter;
            occIter->next = iter;
            return (char*)iter + sizeof(MemBlockDesc);
        }
    }
    //no suitable free blocks found, check if there is enough memory to map
    if((char*)maxSegment + maxSegment->size*MEM_BLOCK_SIZE + size * MEM_BLOCK_SIZE <= (char*)HEAP_END_ADDR) {
        auto* newDesc = (MemBlockDesc*)((char*)maxSegment + maxSegment->size*MEM_BLOCK_SIZE);
        newDesc->size = size;
        newDesc->next = nullptr;
        newDesc->prev = occupiedMemTail;
        newDesc->prev->next = newDesc;
        occupiedMemTail = newDesc;
        maxSegment = newDesc;
        return (char*)newDesc + sizeof(MemBlockDesc);
    }
    return nullptr; //not enough memory
}

int MemoryAllocator::kmem_free(void* addr) {
    for(MemBlockDesc* iter = occupiedMemHead; iter != nullptr; iter = iter->next) { //search for block to be freed
        if ((char*) iter + sizeof(MemBlockDesc) == (char*)addr) { //found it
            if (iter->next != nullptr) iter->next->prev = iter->prev;
            else occupiedMemTail = iter->prev;
            iter->prev->next = iter->next;

            if (freeMemHead == nullptr || freeMemHead > iter) { //insert into freeMemList sorted by address
                iter->next = freeMemHead;
                if(iter->next != nullptr) iter->next->prev = iter;
                iter->prev = nullptr;
                freeMemHead = iter;
                tryMerge(iter);
                return 0;

            }
            MemBlockDesc *freeIter = freeMemHead;
            for (; freeIter->next != nullptr && freeIter->next < iter; freeIter = freeIter->next);
            iter->next = freeIter->next;
            iter->prev = freeIter;
            if (freeIter->next != nullptr) freeIter->next->prev = iter;
            freeIter->next = iter;
            tryMerge(iter);
            tryMerge(freeIter);
            return 0;
        }
    }
    return -1; //no block found
}