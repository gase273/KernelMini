//
// Created by os on 6/23/22.
//

#include "../h/TCB.h"
#include "../h/SysRegInterface.hpp"

TCB* TCB::runningThread = nullptr;
size_t TCB::tSliceCount = 0;

void TCB::yield() {
    __asm__ volatile ("li a0, 0x13");
    __asm__ volatile ("ecall");
}

void TCB::dispatch() {
    TCB* oldRunning = runningThread;
    if(!oldRunning->end) Scheduler::put(oldRunning);
    runningThread = Scheduler::get();
    TCB::switchContext(&oldRunning->context, &runningThread->context);
}

void *TCB::operator new(size_t size) {
    size_t blockSize = (size + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
    if(((size + MemoryAllocator::MemBlockDescSize + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE) > blockSize)
        blockSize = (size + MemoryAllocator::MemBlockDescSize + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
    MemoryAllocator* instance = MemoryAllocator::getInstance();
    return instance->kmem_alloc(blockSize);
}

void TCB::operator delete(void *addr) {
    MemoryAllocator* instance = MemoryAllocator::getInstance();
    instance->kmem_free(addr);
}

TCB* TCB::createUserThread(func function, void* arg, bool start) {
    return new TCB(function, arg, false, start);
}

TCB* TCB::createSystemThread(func function, void* arg) {
    return new TCB(function, arg, true, true);
}

void TCB::deleteThread(TCB* thread) {
    delete thread;
}

void TCB::wrapper() { //called from interrupt
    if(!runningThread->system) SysReg::clrMaskSstatus(SysReg::SSTATUS_SPP);
    else SysReg::setMaskSstatus(SysReg::SSTATUS_SPP);
    SysReg::interruptRet();
    runningThread->threadFunction(runningThread->fArg);
    runningThread->setAnEnd(true);
    TCB::yield();
}
