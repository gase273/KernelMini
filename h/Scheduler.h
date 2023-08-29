//
// Created by os on 6/23/22.
//

#ifndef PROJECT_BASE_V1_1_SCHEDULER_H
#define PROJECT_BASE_V1_1_SCHEDULER_H

class TCB;

class Scheduler { // FIFO queue with time sharing
private:
    static TCB* ReadyThreadListHead;
    static TCB* ReadyThreadListTail;

    static TCB* SleepingThreadListHead;

    static void putSleep(TCB* thread);
    static TCB* getSleep();

public:
    static TCB* get();
    static void put(TCB* thread);
    static void putFront(TCB* thread);

    Scheduler() = delete;
    static void sleep(unsigned long _100ms);
    static void wake();
    static void decrementSleep();

};


#endif //PROJECT_BASE_V1_1_SCHEDULER_H
