//
// Created by matejbucek on 31.8.22.
//

#pragma once

namespace Utils::Lock {
    enum class LockState {
        UNLOCKED = 0,
        LOCKED = 1
    };
    class Lock {
    protected:
        LockState state;
    public:
        Lock();
        ~Lock();
        void sleep();
        void spin();
        bool tryLock();
        void unlock();
    };
}