//
// Created by os on 6/24/22.
//

#include "../h/SysRegInterface.hpp"

void SysReg::interruptRet() {
    __asm__ volatile ("csrw sepc, ra");
    __asm__ volatile ("sret");
}