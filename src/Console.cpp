//
// Created by os on 6/29/22.
//
#include "../h/Console.h"
#include "../h/Sem.h"
#include "../h/BinSem.h"
#include "../h/MemoryAllocator.h"
#include "../h/TCB.h"
#include "../h/SysRegInterface.hpp"

KConsole* KConsole::instance = nullptr;

void KConsole::putc(char chr) {
    instance->spaceAvailableOut->acquire();
    instance->mutexOut->acquire();
    instance->outBuffer[instance->tailOut] = chr;
    instance->tailOut = (instance->tailOut + 1) % 2048;
    instance->mutexOut->release();
    instance->itemAvailableOut->release();
}

char KConsole::getc() {
    instance->itemAvailableIn->acquire();
    instance->mutexIn->acquire();
    char result = instance->inBuffer[instance->headIn];
    instance->headIn = (instance->headIn + 1) % 2048;
    instance->mutexIn->release();
    instance->spaceAvailableIn->release();
    return result;
}

void *KConsole::operator new(size_t size) {
    size_t blockSize = (size + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
    if(((size + MemoryAllocator::MemBlockDescSize + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE) > blockSize)
        blockSize = (size + MemoryAllocator::MemBlockDescSize + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
    MemoryAllocator* allocator = MemoryAllocator::getInstance();
    return allocator->kmem_alloc(blockSize);
}

void KConsole::operator delete(void* addr) {
    MemoryAllocator* allocator = MemoryAllocator::getInstance();
    allocator->kmem_free(addr);
}

KConsole *KConsole::getInstance() {
    if (instance == nullptr) {
        instance = new KConsole;
    }
    return instance;
}

void KConsole::deleteInstance() {
    delete instance;
}

KConsole::KConsole() : headIn(0), tailIn(0), headOut(0), tailOut(0) {

    systemInThread = TCB::createSystemThread(systemInThreadBody, nullptr);
    systemOutThread = TCB::createSystemThread(systemOutThreadBody, nullptr);

    itemAvailableIn = Sem::createSem(0);
    itemAvailableOut = Sem::createSem(0);
    spaceAvailableOut = Sem::createSem(2048);
    spaceAvailableIn = Sem::createSem(2048);
    mutexIn = BinarySem::createBinSem(true);
    mutexOut = BinarySem::createBinSem(true);
    interruptSemIn = BinarySem::createBinSem(true);
    interruptSemOut = BinarySem::createBinSem(true);
}

KConsole::~KConsole() {
    MemoryAllocator* allocator = MemoryAllocator::getInstance();

    allocator->kmem_free(inBuffer);
    allocator->kmem_free(outBuffer);

    Sem::deleteSem(itemAvailableIn);
    Sem::deleteSem(itemAvailableOut);
    Sem::deleteSem(spaceAvailableOut);

    BinarySem::deleteBinSem(mutexIn);
    BinarySem::deleteBinSem(mutexOut);
    BinarySem::deleteBinSem(interruptSemOut);
    BinarySem::deleteBinSem(interruptSemIn);

    TCB::deleteThread(systemInThread);
    TCB::deleteThread(systemOutThread);
}

void KConsole::systemOutThreadBody(void*) {

    static volatile char* Status = (char*)CONSOLE_STATUS;
    static volatile char* outData = (char*)CONSOLE_TX_DATA;
    while(true) {
        while(*Status & CONSOLE_TX_STATUS_BIT) {
            getInstance()->itemAvailableOut->acquire();
            getInstance()->mutexOut->acquire();
            *outData = getInstance()->outBuffer[getInstance()->headOut];
            getInstance()->headOut = (getInstance()->headOut + 1) % 2048;
            getInstance()->mutexOut->release();
            getInstance()->spaceAvailableOut->release();
        }
        getInstance()->interruptSemOut->acquire();
    }
}

void KConsole::systemInThreadBody(void*) {
    static volatile char* Status = (char*)CONSOLE_STATUS;
    static volatile char* inData = (char*)CONSOLE_RX_DATA;
    while(true) {
        while(*Status & CONSOLE_RX_STATUS_BIT) {
            getInstance()->spaceAvailableIn->acquire();
            getInstance()->mutexIn->acquire();
            instance->inBuffer[instance->tailIn] = *inData;
            getInstance()->tailIn = (getInstance()->tailIn + 1) % 2048;
            getInstance()->mutexIn->release();
            getInstance()->itemAvailableIn->release();
        }
        getInstance()->interruptSemIn->acquire();
    }
}
