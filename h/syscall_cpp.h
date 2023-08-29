//
// Created by os on 6/30/22.
//

#ifndef PROJECT_BASE_V1_1_SYSCALL_CPP_H
#define PROJECT_BASE_V1_1_SYSCALL_CPP_H

#ifndef _syscall_cpp
#define _syscall_cpp
#include "syscall_c.h"

void* operator new (size_t);
void operator delete (void*) noexcept;

class Thread {
public:
    Thread (void (*body)(void*), void* arg);
    virtual ~Thread ();
    int start ();
    static void dispatch ();
    static int sleep (time_t);

protected:
    Thread ();
    virtual void run () {}

private:
    static void wrapper(void*);
    thread_t myHandle = nullptr;
};

class Semaphore {
public:
    explicit Semaphore (unsigned init = 1);
    virtual ~Semaphore ();
    int wait ();
    int signal ();
private:
    sem_t myHandle = nullptr;
};

class PeriodicThread : public Thread {
protected:
    explicit PeriodicThread (time_t period);
    void run() override;
    virtual void periodicActivation () {}
private:
    time_t sleepPeriod;
};

class Console {
public:
    static char getc ();
    static void putc (char);
};
#endif

#endif //PROJECT_BASE_V1_1_SYSCALL_CPP_H
