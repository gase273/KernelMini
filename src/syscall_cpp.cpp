//
// Created by os on 6/30/22.
//
#include "../h/syscall_cpp.h"

void Thread::wrapper(void* t) { if(t != nullptr) ((Thread*)t)->run(); }

void* operator new(size_t size) {
    return mem_alloc(size);
}

void operator delete(void* addr) noexcept {
    if(addr) mem_free(addr);
}

Thread::Thread(void (*body)(void *), void *arg) {
    thread_create_no_start(&myHandle,body, arg);
}

Thread::~Thread() {
    if(myHandle != nullptr) thread_delete(myHandle);
}

void Thread::dispatch() {
    thread_dispatch();
}

int Thread::sleep(time_t time) {
    return time_sleep(time);
}

int Thread::start() {
    if(myHandle != nullptr) {
        return thread_start(myHandle);
    }
    else return -1;
}

Thread::Thread() {
    thread_create_no_start(&myHandle, &wrapper, this);
}

Semaphore::Semaphore(unsigned int init) {
    sem_open(&myHandle, init);
}

Semaphore::~Semaphore() {
    sem_close(myHandle);
}

int Semaphore::wait() {
    return sem_wait(myHandle);
}

int Semaphore::signal() {
    return sem_signal(myHandle);
}

char Console::getc() {
    return ::getc();
}

void Console::putc(char chr) {
    ::putc(chr);
}

PeriodicThread::PeriodicThread(time_t period) {
    sleepPeriod = period;
}

void PeriodicThread::run() {
    periodicActivation();
    time_sleep(sleepPeriod);
}