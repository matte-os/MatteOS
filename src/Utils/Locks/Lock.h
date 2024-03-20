//
// Created by matejbucek on 31.8.22.
//

#pragma once

#include "Utils/DebugConsole.hh"
namespace Utils::Locks {
    enum class LockState: u8 {
        UNLOCKED = 0,
        LOCKED = 1
    };

    class SpinLock {
    protected:
        LockState m_state;
    public:
        SpinLock() : m_state(LockState::UNLOCKED) {}
        void unlock() {
            DebugConsole::println("Is about unlocking!");
            DebugConsole::print_ln_number((int)m_state, 16);
            asm volatile("amoswap.d.rl zero, zero, (%0)" :: "r"(&m_state));
            DebugConsole::println("Unlocked");
        }
        bool try_lock() {
            LockState st;
            asm volatile("amoswap.d.aq %0, %1, (%2)" : "=r"(m_state) : "r"(1), "r"(this));
            return st == LockState::UNLOCKED;
        }
        ~SpinLock() {
          DebugConsole::println("Destroing the lock");
        }
    };

    //class Mutex : public Lock {
    //};
}
