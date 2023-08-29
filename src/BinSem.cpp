//
// Created by os on 6/29/22.
//

#include "../h/TCB.h"
#include "../h/BinSem.h"
#include "../h/SysRegInterface.hpp"


/*extern "C" void pushReg();
extern "C" void popReg();*/

void BinarySem::block() {
    TCB* oldRunning = TCB::runningThread;
    put(oldRunning);
    TCB::runningThread = Scheduler::get();
    TCB::switchContext(&oldRunning->context, &TCB::runningThread->context);
}

void BinarySem::unblock() {
    TCB* unblocked = get();
    if (unblocked->isSystem()) Scheduler::putFront(unblocked);
    else Scheduler::put(unblocked);
}


void BinarySem::put(TCB *thread) {
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

TCB *BinarySem::get() {
    TCB* temp = BlockedThreadListHead;
    BlockedThreadListHead = temp->next;
    if (BlockedThreadListHead != nullptr) BlockedThreadListHead->prev = nullptr;
    else BlockedThreadListTail = nullptr;
    temp->next = nullptr;
    return temp;
}

void BinarySem::acquire() {
    volatile uint64 sstatusOnStack = SysReg::rdSstatus();
    if(!val) block(); //interrupts are disabled in supervisor mode so this is atomic
    else val = false;
    SysReg::wrSstatus(sstatusOnStack);
}

void BinarySem::release() {
    if(BlockedThreadListHead != nullptr) unblock(); //interrupts are disabled in supervisor mode so this is atomic
    else val = true;
}

void *BinarySem::operator new(size_t size) {
    size_t blockSize = (size + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
    if(((size + MemoryAllocator::MemBlockDescSize + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE) > blockSize)
        blockSize = (size + MemoryAllocator::MemBlockDescSize + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
    MemoryAllocator* instance = MemoryAllocator::getInstance();
    return instance->kmem_alloc(blockSize);
}

void BinarySem::operator delete(void* addr) {
    MemoryAllocator* instance = MemoryAllocator::getInstance();
    instance->kmem_free(addr);
}

BinarySem *BinarySem::createBinSem(bool startVal) {
    return new BinarySem(startVal);
}

void BinarySem::deleteBinSem(BinarySem* sem) {
    delete sem;
}

BinarySem::~BinarySem() {
    while(BlockedThreadListHead != nullptr) {
        TCB* thread = get();
        thread->setInterrupt(true);
        Scheduler::put(thread);
    }
}

