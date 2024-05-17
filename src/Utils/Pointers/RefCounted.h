//
// Created by matejbucek on 10.9.22.
//

#pragma once

namespace Utils::Pointers {
    class RefCountedBase{
    public:
        unsigned int mutable m_ref_count = 0;
    public:
        void ref() const {
            ++m_ref_count;
        }

        unsigned int deref() const {
            return --m_ref_count;
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