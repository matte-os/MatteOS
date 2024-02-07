//
// Created by matejbucek on 30.10.22.
//

#pragma once

#include <Utils/Locks/Lock.h>

namespace Utils::Locks {
    template<typename T, typename L = SpinLock>
    class Locked{
    private:
        T value;
        L lock;
    public:
        T operator->() {
            lock.use();
            return value;
        }
    };
}
