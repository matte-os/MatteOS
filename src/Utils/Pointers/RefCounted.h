//
// Created by matejbucek on 10.9.22.
//

#pragma once

namespace Utils::Pointers {
    class RefCountedBase{
    public:
        unsigned int mutable refCount = 0;
    public:
        void ref() const {
            ++refCount;
        }

        unsigned int deref() const {
            return --refCount;
        }
    protected:
        RefCountedBase() = default;
        ~RefCountedBase() = default;
    };

    template<typename T>
    class RefCounted : public RefCountedBase {
    public:
        bool unref() const {
            if(deref() == 0) {
                delete static_cast<T const*>(this);
                return true;
            }
            return false;
        }
    };
}

using Utils::Pointers::RefCounted;