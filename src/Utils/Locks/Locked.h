/**
 * @file Locked.h
 * @author Matěj Bucek (matejbucek)
 */
#pragma once

#include <Utils/Locks/Lock.h>

namespace Utils::Locks {
    template<typename T, typename Lock = SpinLock>
    class Locked{
    private:
        T value;
        Lock lock;
    public:
        T operator->() {
            lock.use();
            return value;
        }
    };
}
