//
// Created by os on 6/27/22.
//

#ifndef PROJECT_BASE_V1_1_CONSOLE_H
#define PROJECT_BASE_V1_1_CONSOLE_H
#include "../lib/hw.h"

class TCB;
class Sem;
class BinarySem;

class KConsole {
private:
    static KConsole* instance;

    static void systemInThreadBody(void*);
    static void systemOutThreadBody(void*);

    TCB* systemInThread; //getc
    TCB* systemOutThread; //putc
    char inBuffer[2048];
    char outBuffer[2048];
    uint8 headIn, tailIn, headOut, tailOut;

    KConsole();
    ~KConsole();

    void* operator new(size_t size);
    void operator delete(void*);

public:

    KConsole(const KConsole&) = delete;
    KConsole(KConsole&&) = delete;
    KConsole& operator = (const KConsole&) = delete;
    KConsole& operator = (KConsole&&) = delete;

    static KConsole* getInstance();
    static void deleteInstance();

    static void putc(char chr);
    static char getc();

    Sem* itemAvailableOut;
    Sem* spaceAvailableOut;
    Sem* itemAvailableIn;
    Sem* spaceAvailableIn;

    BinarySem* mutexIn;
    BinarySem* mutexOut;
    BinarySem* interruptSemIn;
    BinarySem* interruptSemOut;
};


#endif //PROJECT_BASE_V1_1_CONSOLE_H
