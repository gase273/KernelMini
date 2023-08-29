//
// Created by os on 6/22/22.
//

#include "../h/syscall_c.h"

char callWrapper(char code) {
    __asm__ volatile("ecall");
    return code;
}

void* mem_alloc(size_t size) {
    __asm__ volatile("mv a1, %0" : : "r"(size));
    __asm__ volatile ("li a0, 0x01");
    __asm__ volatile("ecall");
    return (void*)size;
};

int mem_free(void* addr) {;
    __asm__ volatile("mv a1, %0" : : "r"(addr));
    __asm__ volatile ("li a0, 0x02");
    __asm__ volatile("ecall");
    volatile int result;
    __asm__ volatile ("mv %0, a0" : "=r"(result));
    return result;
}

int thread_create(thread_t *handle, void (*start_routine)(void *), void *arg) {
    __asm__ volatile("mv a3, %0" : : "r"(arg));
    __asm__ volatile("mv a2, %0" : : "r"(start_routine));
    __asm__ volatile("mv a1, %0" : : "r"(handle));
    __asm__ volatile("li a0, 0x11");
    __asm__ volatile("ecall");
    volatile int result;
    __asm__ volatile ("mv %0, a0" : "=r"(result));
    return result;
}

void thread_dispatch() {
    __asm__ volatile("li a0, 0x13");
    __asm__ volatile("ecall");
}

int thread_exit() {
    __asm__ volatile("li a0, 0x12");
    __asm__ volatile("ecall");
    volatile int result;
    __asm__ volatile("mv %0, a0" : "=r"(result));
    return result;
}

void thread_delete(thread_t handle) {
    __asm__ volatile("mv a1, %0" : : "r"(handle));
    __asm__ volatile("li a0, 0x14");
    __asm__ volatile("ecall");
}

int sem_open(sem_t *handle, unsigned int init) {
    __asm__ volatile("mv a2, %0" : : "r"((uint64)init));
    __asm__ volatile("mv a1, %0" : : "r"(handle));
    __asm__ volatile("li a0, 0x21");
    __asm__ volatile("ecall");
    volatile int result;
    __asm__ volatile("mv %0, a0" : "=r"(result));
    return result;
}

int sem_close(sem_t handle) {
    __asm__ volatile("mv a1, %0" : : "r"(handle));
    __asm__ volatile("li a0, 0x22");
    __asm__ volatile("ecall");
    volatile int result;
    __asm__ volatile("mv %0, a0" : "=r"(result));
    return result;
}

int sem_wait(sem_t id) {
    __asm__ volatile("mv a1, %0" : : "r"(id));
    __asm__ volatile("li a0, 0x23");
    __asm__ volatile("ecall");
    volatile int result;
    __asm__ volatile("mv %0, a0" : "=r"(result));
    return result;
}

int sem_signal(sem_t id) {
    __asm__ volatile("mv a1, %0" : : "r"(id));
    __asm__ volatile("li a0, 0x24");
    __asm__ volatile("ecall");
    volatile int result;
    __asm__ volatile("mv %0, a0" : "=r"(result));
    return result;
}

int time_sleep(time_t _100ms) {
    if (_100ms == 0) return 0;
    __asm__ volatile("mv a1, %0" : : "r"(_100ms));
    __asm__ volatile("li a0, 0x31");
    __asm__ volatile("ecall");
    volatile int result;
    __asm__ volatile("mv %0, a0" : "=r"(result));
    return result;
}

char getc() {
    return callWrapper(0x41);
}

void putc(char chr) {
    __asm__ volatile("mv a1, %0" : : "r"(chr));
    __asm__ volatile("li a0, 0x42");
    __asm__ volatile("ecall");
}

int thread_create_no_start(thread_t *handle, void (*start_routine)(void *), void *arg) {
    __asm__ volatile("mv a3, %0" : : "r"(arg));
    __asm__ volatile("mv a2, %0" : : "r"(start_routine));
    __asm__ volatile("mv a1, %0" : : "r"(handle));
    __asm__ volatile("li a0, 0x15");
    __asm__ volatile("ecall");
    volatile int result;
    __asm__ volatile ("mv %0, a0" : "=r"(result));
    return result;
}

int thread_start(thread_t handle) {
    __asm__ volatile("mv a1, %0" : : "r"(handle));
    __asm__ volatile("li a0, 0x16");
    __asm__ volatile("ecall");
    volatile int result;
    __asm__ volatile ("mv %0, a0" : "=r"(result));
    return result;
}
