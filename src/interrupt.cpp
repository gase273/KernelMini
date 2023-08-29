//
// Created by os on 6/22/22.
//

#include "../h/interrupt.h"
#include "../h/SysRegInterface.hpp"
#include "../h/sisprint.h"
#include "../h/MemoryAllocator.h"
#include "../h/TCB.h"
#include "../h/Console.h"

extern void consHandler();

extern "C" void SupervisorIntHandler() {
    volatile uint64 scauseVal;
    scauseVal = SysReg::rdScause();
    switch(scauseVal) {
        case SysReg::SCAUSE_TIMER: {
            TCB::tSliceCount++;
            Scheduler::decrementSleep();
            Scheduler::wake();
            if (TCB::tSliceCount >= TCB::runningThread->tSlice) {
                TCB::tSliceCount = 0;
                volatile uint64 sscratchOnStack = SysReg::rdSscratch();
                volatile uint64 sepcOnStack = SysReg::rdSepc();
                volatile uint64 sstatusOnStack = SysReg::rdSstatus(); //easiest way to save sysregs on stack
                TCB::dispatch();
                SysReg::wrSstatus(sstatusOnStack);
                SysReg::wrSepc(sepcOnStack); //written sepc and sstatus from stack
                SysReg::wrSscratch(sscratchOnStack);
            }

            SysReg::clrMaskSip(SysReg::SIP_SSIP);
            break;
        }

        case SysReg::SCAUSE_EXTERN: {
            int useless = plic_claim();
            if (useless == 10)  consHandler();
            plic_complete(useless);
            break;
        }

        case SysReg::SCAUSE_ECALLS: //TODO Branch this out later
        case SysReg::SCAUSE_ECALLU: {
            __asm__ volatile ("csrw sscratch, s0");
            volatile uint64 code;
            __asm__ volatile ("mv %0, a0" : "=r"(code));
            branch(code);
            volatile uint64 newSepc = SysReg::rdSepc();
            newSepc += 4;
            SysReg::wrSepc(newSepc);
            break;
        }

        case SysReg::SCAUSE_ILLEGAL:
        case SysReg::SCAUSE_NORD:
        case SysReg::SCAUSE_NOWR: {
            printSisStr("Error!\n");
            printSisUInt64(SysReg::rdScause());
            printSisStr("\n");
            printSisUInt64(SysReg::rdStval());
            printSisStr("\n");
            printSisUInt64(SysReg::rdSepc());
            printSisStr("\n");
            printSisUInt64(SysReg::rdSstatus());
            break;
        }
        default: break;
    }
}

void branch(volatile uint64 callCode) {
    switch(callCode) {
        case 0x01UL: { //mem_alloc
            volatile size_t byteSize;
            __asm__ volatile ("mv %0, a1" : "=r"(byteSize));
            size_t blockSize = (byteSize + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
            if(((byteSize + MemoryAllocator::MemBlockDescSize + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE) > blockSize)
                blockSize = (byteSize + MemoryAllocator::MemBlockDescSize + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
            MemoryAllocator* instance = MemoryAllocator::getInstance();
            void* data = instance->kmem_alloc(blockSize);
            __asm__ volatile ("csrr t1, sscratch");
            __asm__ volatile ("sd %0, 80(t1)" : : "r"(data));
            break;
        }

        case 0x02UL: { //mem_free
            void* addr;
            __asm__ volatile ("mv %0, a1" : "=r"(addr));
            MemoryAllocator* instance = MemoryAllocator::getInstance();
            int free = instance->kmem_free(addr);
            __asm__ volatile ("csrr t1, sscratch");
            __asm__ volatile ("sd %0, 80(t1) " : : "r"(free));
            break;
        }

        case 0x11UL: { //thread_create
            TCB** handle;
            TCB::func function;
            void* arg;
            __asm__ volatile ("mv %0, a1" : "=r"(handle));
            __asm__ volatile ("mv %0, a2" : "=r"(function));
            __asm__ volatile ("mv %0, a3" : "=r"(arg));
            *handle = TCB::createUserThread(function, arg, true);
            __asm__ volatile ("csrr t1, sscratch");
            __asm__ volatile ("sd %0, 80(t1) " : : "r"(0UL));
            break;
        }

        case 0x12UL: { //thread_exit;
            TCB::tSliceCount = 0;
            volatile uint64 sscratchOnStack = SysReg::rdSscratch();
            volatile uint64 sepcOnStack = SysReg::rdSepc();
            volatile uint64 sstatusOnStack = SysReg::rdSstatus(); //easiest way to save sysregs on stack
            TCB::runningThread->setAnEnd(true);
            TCB::dispatch();
            SysReg::wrSstatus(sstatusOnStack);
            SysReg::wrSepc(sepcOnStack); //written sepc and sstatus from stack
            SysReg::wrSscratch(sscratchOnStack);
            __asm__ volatile ("csrr t1, sscratch");
            __asm__ volatile ("sd %0, 80(t1) " : : "r"(0UL));
            break;
        }

        case 0x13UL: { //thread_dispatch (yield);
            TCB::tSliceCount = 0;
            volatile uint64 sscratchOnStack = SysReg::rdSscratch();
            volatile uint64 sepcOnStack = SysReg::rdSepc();
            volatile uint64 sstatusOnStack = SysReg::rdSstatus(); //easiest way to save sysregs on stack
            TCB::dispatch();
            SysReg::wrSstatus(sstatusOnStack);
            SysReg::wrSepc(sepcOnStack); //written sepc and sstatus from stack
            SysReg::wrSscratch(sscratchOnStack);
            break;
        }

        case 0x14UL: { //thread_delete
            TCB* handle;
            __asm__ volatile("mv %0, a1" : "=r"(handle));
            if (handle->isAnEnd()) TCB::deleteThread(handle);
            break;
        }

        case 0x15UL: { //thread_create_no_start
            TCB** handle;
            TCB::func function;
            void* arg;
            __asm__ volatile ("mv %0, a1" : "=r"(handle));
            __asm__ volatile ("mv %0, a2" : "=r"(function));
            __asm__ volatile ("mv %0, a3" : "=r"(arg));
            *handle = TCB::createUserThread(function, arg, false);
            __asm__ volatile ("csrr t1, sscratch");
            __asm__ volatile ("sd %0, 80(t1) " : : "r"(0UL));
            break;
        }

        case 0x16UL: { //thread_start
            TCB* handle;
            long result;
            __asm__ volatile ("mv %0, a1" : "=r"(handle));
            if(!handle->isStarted()) {
                Scheduler::put(handle);
                result = 0;
            }
            else result = 1;
            __asm__ volatile ("csrr t1, sscratch");
            __asm__ volatile ("sd %0, 80(t1) " : : "r"(result));
            break;
        }

        case 0x21UL: { //semaphore_open
            Sem** handle;
            uint32 start;
            __asm__ volatile ("mv %0, a1" : "=r"(handle));
            __asm__ volatile ("mv %0, a2" : "=r"(start));
            *handle = Sem::createSem(start);
            __asm__ volatile ("csrr t1, sscratch");
            __asm__ volatile ("sd %0, 80(t1) " : : "r"(0UL));
            break;
        }

        case 0x22UL: { //semaphore_close
            Sem* handle;
            __asm__ volatile ("mv %0, a1" : "=r"(handle));
            Sem::deleteSem(handle);
            __asm__ volatile ("csrr t1, sscratch");
            __asm__ volatile ("sd %0, 80(t1) " : : "r"(0UL));
            break;
        }

        case 0x23UL: { //semaphore_wait
            Sem* handle;
            __asm__ volatile ("mv %0, a1" : "=r"(handle));
            TCB::tSliceCount = 0;
            volatile uint64 sscratchOnStack = SysReg::rdSscratch();
            volatile uint64 sepcOnStack = SysReg::rdSepc();
            volatile uint64 sstatusOnStack = SysReg::rdSstatus();
            handle->acquire();
            SysReg::wrSstatus(sstatusOnStack);
            SysReg::wrSepc(sepcOnStack);
            SysReg::wrSscratch(sscratchOnStack);
            __asm__ volatile ("csrr t1, sscratch");
            int result = TCB::runningThread->interrupt ? -1 : 0;
            __asm__ volatile ("sd %0, 80(t1) " : : "r"(result));
            TCB::runningThread->setInterrupt(false);
            break;
        }

        case 0x24UL: { //semaphore_signal
            Sem* handle;
            __asm__ volatile ("mv %0, a1" : "=r"(handle));
            handle->release();
            __asm__ volatile ("csrr t1, sscratch");
            __asm__ volatile ("sd %0, 80(t1) " : : "r"(0UL));
            break;
        }

        case 0x31UL: { //time_sleep
            uint64 time;
            __asm__ volatile ("mv %0, a1" : "=r"(time));
            TCB::tSliceCount = 0;
            volatile uint64 sscratchOnStack = SysReg::rdSscratch();
            volatile uint64 sepcOnStack = SysReg::rdSepc();
            volatile uint64 sstatusOnStack = SysReg::rdSstatus();
            Scheduler::sleep(time);
            SysReg::wrSstatus(sstatusOnStack);
            SysReg::wrSepc(sepcOnStack);
            SysReg::wrSscratch(sscratchOnStack);
            __asm__ volatile ("csrr t1, sscratch");
            __asm__ volatile ("sd %0, 80(t1) " : : "r"(0UL));
            break;
        }

       case 0x41UL: { //getc
            volatile uint64 sscratchOnStack = SysReg::rdSscratch();
            volatile uint64 sepcOnStack = SysReg::rdSepc();
            volatile uint64 sstatusOnStack = SysReg::rdSstatus();
            char res = KConsole::getc();
            SysReg::wrSstatus(sstatusOnStack);
            SysReg::wrSepc(sepcOnStack);
            SysReg::wrSscratch(sscratchOnStack);
            __asm__ volatile ("csrr t1, sscratch");
            __asm__ volatile ("sd %0, 80(t1) " : : "r"(res));
            break;
        }

        case 0x42UL: { //putc
            char b;
            __asm__ volatile("mv %0, a1" : "=r"(b));
            volatile uint64 sscratchOnStack = SysReg::rdSscratch();
            volatile uint64 sepcOnStack = SysReg::rdSepc();
            volatile uint64 sstatusOnStack = SysReg::rdSstatus();
            KConsole::putc(b);
            SysReg::wrSstatus(sstatusOnStack);
            SysReg::wrSepc(sepcOnStack);
            SysReg::wrSscratch(sscratchOnStack);
            break;
        }

    }
}