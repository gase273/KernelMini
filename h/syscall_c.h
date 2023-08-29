//
// Created by os on 6/22/22.
//

#ifndef PROJECT_BASE_V1_1_SYSCALL_C_H
#define PROJECT_BASE_V1_1_SYSCALL_C_H

#include "../lib/hw.h"
class TCB;
class Sem;

typedef TCB* thread_t;
typedef Sem* sem_t;
typedef unsigned long time_t;
const int EOF = -1;
#ifdef __cplusplus
extern "C" {
#endif

    void* mem_alloc(size_t size);

    int mem_free(void*);

    int thread_create(thread_t* handle, void(*start_routine)(void*), void* arg);

    int thread_create_no_start(thread_t* handle, void(*start_routine)(void*), void* arg);

    int thread_start(thread_t handle);

    void thread_dispatch();

    int thread_exit();

    void thread_delete(thread_t handle);

    int sem_open(sem_t* handle, unsigned init);

    int sem_close(sem_t handle);

    int sem_wait(sem_t id);

    int sem_signal(sem_t id);

    int time_sleep(time_t _100ms);

    char getc();

    void putc(char chr);


#ifdef __cplusplus
}
#endif

#endif //PROJECT_BASE_V1_1_SYSCALL_C_H
