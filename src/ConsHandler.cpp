//
// Created by os on 6/29/22.
//


#include "../h/Console.h"
#include "../h/Sem.h"
#include "../h/BinSem.h"

void consHandler() {
    KConsole* cons = KConsole::getInstance();
    cons->interruptSemIn->release();
    cons->interruptSemOut->release();
}
