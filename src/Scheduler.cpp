//
// Created by os on 6/23/22.
//
#include "../h/Scheduler.h"
#include "../h/TCB.h"

TCB* Scheduler::ReadyThreadListHead = nullptr;
TCB* Scheduler::ReadyThreadListTail = nullptr;
TCB* Scheduler::SleepingThreadListHead = nullptr;

TCB *Scheduler::get() {
    TCB* temp = ReadyThreadListHead;
    ReadyThreadListHead = temp->next;
    if (ReadyThreadListHead != nullptr) ReadyThreadListHead->prev = nullptr;
    temp->next = nullptr;
    return temp;
}

void Scheduler::put(TCB *thread) {
    if(thread == nullptr) return;
    if (ReadyThreadListHead == nullptr) {
        thread->next = ReadyThreadListHead;
        thread->prev = nullptr;
        thread->setStarted(true);
        ReadyThreadListHead = thread;
        ReadyThreadListTail = thread;
        return;
    }
    ReadyThreadListTail->next = thread;
    thread->prev = ReadyThreadListTail;
    thread->next = nullptr;
    thread->setStarted(true);
    ReadyThreadListTail = thread;
}

void Scheduler::sleep(uint64 _100ms) {
    TCB* oldRunning = TCB::runningThread;
    oldRunning->sleepTime = _100ms;
    putSleep(oldRunning);
    TCB::runningThread = get();
    TCB::switchContext(&oldRunning->context, &TCB::runningThread->context);
}

TCB *Scheduler::getSleep() {
    TCB* temp = SleepingThreadListHead;
    SleepingThreadListHead = temp->next;
    if (SleepingThreadListHead != nullptr) SleepingThreadListHead->prev = nullptr;
    temp->next = nullptr;
    return temp;
}

void Scheduler::putSleep(TCB *thread) {
    if (SleepingThreadListHead == nullptr || thread->sleepTime < SleepingThreadListHead->sleepTime) {
        thread->next = SleepingThreadListHead;
        thread->prev = nullptr;
        if (SleepingThreadListHead != nullptr) {
            SleepingThreadListHead->prev = thread;
            SleepingThreadListHead->sleepTime -= thread->sleepTime;
        }
        SleepingThreadListHead = thread;
        return;
    }
    TCB* iter = SleepingThreadListHead;
    uint64 totalSleepTime = SleepingThreadListHead->sleepTime;
    for(; iter->next != nullptr && (totalSleepTime + iter->next->sleepTime) <= thread->sleepTime; iter = iter->next) {
        totalSleepTime += iter->next->sleepTime;
    }
    thread->sleepTime -= totalSleepTime;
    thread->next = iter->next;
    thread->prev = iter;
    iter->next = thread;
    if (thread->next != nullptr) {
        thread->next->prev = thread;
        thread->next->sleepTime -= thread->sleepTime;
    }
}

void Scheduler::wake() {
    while(SleepingThreadListHead && SleepingThreadListHead->sleepTime == 0) {
        TCB* thread = getSleep();
        put(thread);
    }
}

void Scheduler::decrementSleep() {
    if(SleepingThreadListHead != nullptr) SleepingThreadListHead->sleepTime--;
}

void Scheduler::putFront(TCB *thread) {
    if (thread == nullptr) return;
    thread->next = ReadyThreadListHead;
    thread->prev = nullptr;
    if (ReadyThreadListHead != nullptr) ReadyThreadListHead->prev = thread;
    else ReadyThreadListTail = thread;
    ReadyThreadListHead = thread;
}

