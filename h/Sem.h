//
// Created by os on 6/25/22.
//

#ifndef PROJECT_BASE_V1_1_SEM_H
#define PROJECT_BASE_V1_1_SEM_H
#include "../lib/hw.h"


class TCB;

class Sem {
public:
    explicit Sem(uint32 init=1) : val(init), BlockedThreadListHead(nullptr), BlockedThreadListTail(nullptr) {}

    void acquire();
    void release();

    static Sem* createSem(uint32 startVal);
    static void deleteSem(Sem* sem);

    long value() const { return val; }

protected:
    void block();
    void unblock();
    void put(TCB*);
    TCB* get();

private:
    long val;
    TCB* BlockedThreadListHead;
    TCB* BlockedThreadListTail;

    void* operator new(size_t size);
    void operator delete(void*);
    ~Sem();
};

#endif //PROJECT_BASE_V1_1_SEM_H
