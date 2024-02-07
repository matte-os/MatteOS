//
// Created by matejbucek on 31.8.22.
//

#pragma once

namespace Utils::Locks {
    enum class LockState {
        UNLOCKED = 0,
        LOCKED = 1
    };

    class Lock {
    protected:
        LockState m_state;
    public:
        Lock() : m_state(LockState::UNLOCKED) {}
        virtual ~Lock() = default;
        virtual void use() = 0;
        virtual void unlock() {
            asm volatile("amoswap.d.rl zero, zero, (%0)" :: "r"(m_state));
        }
        bool try_lock() {
            LockState st;
            asm volatile("amoswap.d.aq %0, %1, (%2)" : "=r"(m_state) : "r"(1), "r"(this));
            return st == LockState::UNLOCKED;
        }
    };

    class SpinLock : public Lock {
    public:
        void use() override {
            while(!try_lock());
        }
        ~SpinLock() = default;
    };

    //class Mutex : public Lock {
    //};
}