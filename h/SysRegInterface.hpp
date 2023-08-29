//
// Created by os on 6/20/22.
//

#include "../lib/hw.h"

#ifndef PROJECT_BASE_V1_1_SYSREGINTERFACE_HPP
#define PROJECT_BASE_V1_1_SYSREGINTERFACE_HPP

class SysReg {
public:
    //sepc register
    static uint64 rdSepc();
    static void wrSepc(uint64 val);

    //scause register and values
    static uint64 rdScause();
    static void wrScause(uint64 val);

    enum ScauseVals {
        SCAUSE_TIMER = (1UL << 63 | 1),
        SCAUSE_EXTERN = (1UL << 63 | 0b1001),
        SCAUSE_ILLEGAL = (1 << 1),
        SCAUSE_NORD = (0b101),
        SCAUSE_NOWR = (0b111),
        SCAUSE_ECALLU = (0b1000),
        SCAUSE_ECALLS = (0b1001),
    };

    //stvec register
    static uint64 rdStvec();
    static void wrStvec(uint64 val);

    //stval register
    static uint64 rdStval();
    static void wrStval(uint64 val);

    //sip register and bit-masks;
    static uint64 rdSip();
    static void wrSip(uint64 val);
    static void setMaskSip(uint64 mask);
    static void clrMaskSip(uint64 mask);

    enum SipBits {
        SIP_SSIP = (1 << 1),
        SIP_STIP = (1 << 5),
        SIP_SEIP = (1 << 9),
    };

    //sie register and bit-masks;
    static uint64 rdSie();
    static void wrSie(uint64 val);
    static void setMaskSie(uint64 mask);
    static void clrMaskSie(uint64 mask);

    enum SieBits {
        SIE_SSIE = (1 << 1),
        SIE_STIE = (1 << 5),
        SIE_SEIE = (1 << 9),
    };

    //sstatus register and bit-masks;
    static uint64 rdSstatus();
    static void wrSstatus(uint64 val);
    static void setMaskSstatus(uint64 mask);
    static void clrMaskSstatus(uint64 mask);

    enum SstatusBits {
        SSTATUS_SIE = (1 << 1),
        SSTATUS_SPIE = (1 << 5),
        SSTATUS_SPP = (1 << 8),
    };

    static void interruptRet();

    static uint64 rdSscratch();

    static void wrSscratch(uint64 val);
};

inline uint64 SysReg::rdSepc() {
    volatile uint64 val;
    __asm__ volatile ("csrr %[value], sepc" : [value] "=r"(val));
    return val;
}

inline void SysReg::wrSepc(uint64 val) {
    __asm__ volatile ("csrw sepc, %[value]" : : [value] "r"(val));
}

inline uint64 SysReg::rdScause() {
    volatile uint64 val;
    __asm__ volatile ("csrr %[value], scause" : [value] "=r"(val));
    return val;
}

inline void SysReg::wrScause(uint64 val) {
    __asm__ volatile ("csrw scause, %[value]" : : [value] "r"(val));
}

inline uint64 SysReg::rdStvec() {
    volatile uint64 val;
    __asm__ volatile ("csrr %[value], stvec" : [value] "=r"(val));
    return val;
}

inline void SysReg::wrStvec(uint64 val) {
    __asm__ volatile ("csrw stvec, %[value]" : : [value] "r"(val));
}

inline uint64 SysReg::rdStval() {
    volatile uint64 val;
    __asm__ volatile ("csrr %[value], stval" : [value] "=r"(val));
    return val;
}

inline void SysReg::wrStval(uint64 val) {
    __asm__ volatile ("csrw stval, %[value]" : : [value] "r"(val));
}

inline uint64 SysReg::rdSip() {
    volatile uint64 val;
    __asm__ volatile ("csrr %[value], sip" : [value] "=r"(val));
    return val;
}

inline void SysReg::wrSip(uint64 val) {
    __asm__ volatile ("csrw sip, %[value]" : : [value] "r"(val));
}

inline void SysReg::setMaskSip(uint64 mask) {
    __asm__ volatile ("csrs sip, %[value]" : : [value] "r"(mask));
}

inline void SysReg::clrMaskSip(uint64 mask) {
    __asm__ volatile ("csrc sip, %[value]" : : [value] "r"(mask));
}

inline uint64 SysReg::rdSie() {
    volatile uint64 val;
    __asm__ volatile ("csrr %[value], sie" : [value] "=r"(val));
    return val;
}

inline void SysReg::wrSie(uint64 val) {
    __asm__ volatile ("csrw sie, %[value]" : : [value] "r"(val));
}

inline void SysReg::setMaskSie(uint64 mask) {
    __asm__ volatile ("csrs sie, %[value]" : : [value] "r"(mask));
}

inline void SysReg::clrMaskSie(uint64 mask) {
    __asm__ volatile ("csrc sie, %[value]" : : [value] "r"(mask));
}

inline uint64 SysReg::rdSstatus() {
    volatile uint64 val;
    __asm__ volatile ("csrr %[value], sstatus" : [value] "=r"(val));
    return val;
}

inline void SysReg::wrSstatus(uint64 val) {
    __asm__ volatile ("csrw sstatus, %[value]" : : [value] "r"(val));
}

inline void SysReg::setMaskSstatus(uint64 mask) {
    __asm__ volatile ("csrs sstatus, %[value]" : : [value] "r"(mask));
}

inline void SysReg::clrMaskSstatus(uint64 mask) {
    __asm__ volatile ("csrc sstatus, %[value]" : : [value] "r"(mask));
}

inline uint64 SysReg::rdSscratch() {
    volatile uint64 val;
    __asm__ volatile ("csrr %[value], sscratch" : [value] "=r"(val));
    return val;
}

inline void SysReg::wrSscratch(uint64 val) {
    __asm__ volatile ("csrw sscratch, %[value]" : : [value] "r"(val));
}

#endif //PROJECT_BASE_V1_1_SYSREGINTERFACE_HPP
