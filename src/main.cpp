//
// Created by os on 6/20/22.
//

#include "../h/syscall_c.h"
#include "../h/SysRegInterface.hpp"
#include "../h/TCB.h"
//#include "../test/workers.h"
#include "../h/interrupt.h"
#include "../h/Console.h"

extern void userMain();

void userMainWrapper(void* arg) {
    userMain();
}

void main() {
    TCB* threads[2];
    KConsole::getInstance();
    threads[0] = TCB::createSystemThread(nullptr,nullptr);
    TCB::runningThread = threads[0];
    threads[1] = TCB::createUserThread(userMainWrapper,nullptr, true);
    SysReg::wrStvec((uint64)&SupervisorInt);
    SysReg::setMaskSstatus(SysReg::SSTATUS_SIE);

    while(!threads[1]->isAnEnd()){ thread_dispatch(); }

	KConsole::deleteInstance();

    for (auto &thread : threads) {
        TCB::deleteThread(thread);
    }
}