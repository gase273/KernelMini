//
// Created by os on 6/25/22.
//

#include "../h/Sem.h"
#include "../h/TCB.h"
#include "../h/SysRegInterface.hpp"


/*extern "C" void pushReg();
extern "C" void popReg();*/

void Sem::block() {
    TCB* oldRunning = TCB::runningThread;
    put(oldRunning);
    TCB::runningThread = Scheduler::get();
    TCB::switchContext(&oldRunning->context, &TCB::runningThread->context);
}

void Sem::unblock() {
    TCB* unblocked = get();
    if(unblocked->isSystem()) Scheduler::putFront(unblocked);
    else Scheduler::put(unblocked);
}

void Sem::put(TCB *thread) {
    if(thread == nullptr) return;
    if (BlockedThreadListHead == nullptr) {
        thread->next = BlockedThreadListHead;
        thread->prev = nullptr;
        BlockedThreadListHead = thread;
        BlockedThreadListTail = thread;
        return;
    }
    BlockedThreadListTail->next = thread;
    thread->prev = BlockedThreadListTail;
    thread->next = nullptr;
    BlockedThreadListTail = thread;
}

TCB *Sem::get() {
    TCB* temp = BlockedThreadListHead;
    BlockedThreadListHead = temp->next;
    if (BlockedThreadListHead != nullptr) BlockedThreadListHead->prev = nullptr;
    else BlockedThreadListTail = nullptr;
    temp->next = nullptr;
    return temp;
}

void Sem::acquire() {
    volatile uint64 sstatusOnStack = SysReg::rdSstatus();
    if(--val < 0) block(); //interrupts are disabled in supervisor mode so this is atomic
    SysReg::wrSstatus(sstatusOnStack);
}

void Sem::release() {
    if(++val <= 0) unblock(); //interrupts are disabled in supervisor mode so this is atomic
}

void *Sem::operator new(size_t size) {
    size_t blockSize = (size + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
    if(((size + MemoryAllocator::MemBlockDescSize + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE) > blockSize)
        blockSize = (size + MemoryAllocator::MemBlockDescSize + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
    MemoryAllocator* instance = MemoryAllocator::getInstance();
    return instance->kmem_alloc(blockSize);
}

void Sem::operator delete(void* addr) {
    MemoryAllocator* instance = MemoryAllocator::getInstance();
    instance->kmem_free(addr);
}

Sem *Sem::createSem(uint32 startVal) {
    return new Sem(startVal);
}

void Sem::deleteSem(Sem* sem) {
    delete sem;
}

Sem::~Sem() {
    while(BlockedThreadListHead != nullptr) {
        TCB* thread = get();
        thread->setInterrupt(true);
        Scheduler::put(thread);
    }
}
