//
// Created by matejbucek on 31.8.22.
//
#include <Utils/Lock/Lock.h>

namespace Utils::Lock {

    Lock::Lock() {
        state = LockState::UNLOCKED;
    }

    Lock::~Lock() {}

    bool Lock::tryLock() {
        LockState st;
        asm("amoswap.w.aq $0, $1, ($2)\n" : "=r"(st) : "r"(1), "r"(state));
        return st == LockState::UNLOCKED;
    }

    void Lock::spin() {
        while(!tryLock()) {}
    }

    void Lock::sleep() {

    }

    void Lock::unlock() {
        asm("amoswap.w.rl zero, zero, ($0)" :: "r"(state));
    }
}