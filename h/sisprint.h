//
// Created by os on 6/29/22.
//

#ifndef PROJECT_BASE_V1_1_SISPRINT_H
#define PROJECT_BASE_V1_1_SISPRINT_H
#include "../lib/hw.h"


const static char digits[] = "0123456789";

extern "C" void printSisStr(char const* str);

extern "C" void printSisInt64(long integer64);
extern "C" void printSisUInt64(uint64 uintiger64);
extern "C" void printSisInt32(int integer32);


#endif //PROJECT_BASE_V1_1_SISPRINT_H
