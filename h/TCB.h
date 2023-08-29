//
// Created by os on 6/23/22.
//

#ifndef PROJECT_BASE_V1_1_TCB_H
#define PROJECT_BASE_V1_1_TCB_H

#include "../lib/hw.h"
#include "./Scheduler.h"
#include "./Sem.h"
#include "../h/MemoryAllocator.h"

extern "C" void SupervisorIntHandler();
void branch(volatile uint64 callCode);

class TCB {
public:
    using func = void(*)(void*);

    bool isAnEnd() const { return end; }
    void setAnEnd(bool anEnd) { this->end = anEnd; }
    bool isSystem() const { return system; }
    bool isInterrupt() const { return interrupt; }
    void setInterrupt(bool anInterrupt) { this->interrupt = anInterrupt; }

    bool isStarted() const { return started; }

    void setStarted(bool start) { TCB::started = start; }

    static TCB* createUserThread(func function, void* arg, bool);
    static TCB* createSystemThread(func function, void* arg);
    static void deleteThread(TCB* thread);
    static void yield();

    static TCB* runningThread;

    TCB(const TCB&) = delete;
    TCB(TCB&&) = delete;
    ~TCB()  {
        MemoryAllocator* instance = MemoryAllocator::getInstance();
        instance->kmem_free(Stack);
    }
    TCB& operator = (const TCB&) = delete;
    TCB& operator = (TCB&&) = delete;



private:
    typedef struct threadContext {
        uint64 sp;
        uint64 ra;
    } threadContext;

    bool end;
    bool system;
    bool interrupt;
    bool started;
    func threadFunction;
    uint64* Stack;
    threadContext context{};
    size_t tSlice;
    uint64 sleepTime;
    void* fArg;
    TCB* prev;
    TCB* next;

    friend class Scheduler;
    friend class Sem; //had to do it like this :(
    friend class BinarySem;
    friend void ::SupervisorIntHandler();
    friend void ::branch(volatile uint64 callCode);

    void* operator new(size_t size);
    void operator delete(void*);

    TCB(func f, void* arg, bool privileged, bool start) :
    end(false), system(privileged), interrupt(false),started(start), threadFunction(f), tSlice(DEFAULT_TIME_SLICE), sleepTime(0),
    fArg(arg), prev(nullptr), next(nullptr)
    {
        MemoryAllocator* instance = MemoryAllocator::getInstance();
        Stack = f != nullptr ? (uint64*)instance->kmem_alloc(sizeof(uint64)*DEFAULT_STACK_SIZE/MEM_BLOCK_SIZE+1) : nullptr;
        context.ra = f != nullptr ? (uint64)&wrapper : 0;
        context.sp = Stack != nullptr ? (uint64) &Stack[DEFAULT_STACK_SIZE] : 0;
        if (f != nullptr && start) Scheduler::put(this);
    }

    static size_t tSliceCount;

    static void switchContext(threadContext* oldContext, threadContext * newContext);
    static void dispatch();
    static void wrapper();

};

#endif //PROJECT_BASE_V1_1_TCB_H
