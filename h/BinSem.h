//
// Created by os on 6/29/22.
//

#ifndef PROJECT_BASE_V1_1_BINSEM_H
#define PROJECT_BASE_V1_1_BINSEM_H
#include "../lib/hw.h"


class TCB;

class BinarySem {
public:
    explicit BinarySem(bool empty = true) : val(empty), BlockedThreadListHead(nullptr), BlockedThreadListTail(nullptr) {}

    void acquire();
    void release();

    static BinarySem* createBinSem(bool startVal);
    static void deleteBinSem(BinarySem* sem);

    long value() const { return val; }

private:

    void block();
    void unblock();
    void put(TCB*);
    TCB* get();

    bool val;
    TCB* BlockedThreadListHead;
    TCB* BlockedThreadListTail;

    void* operator new(size_t size);
    void operator delete(void*);
    ~BinarySem();
};

#endif //PROJECT_BASE_V1_1_BINSEM_H
