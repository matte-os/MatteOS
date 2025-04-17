/**
 * @file RefCounted.h
 * @author Matěj Bucek (matejbucek)
 *
 * This file was inspired by SerenityOS.
 */
#pragma once

namespace Utils {
    class RefCountedBase{
    public:
        unsigned int mutable m_ref_count = 0;
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
        virtual ~RefCounted() = default;

        bool unref() const {
            if(deref() == 0) {
                delete static_cast<T const*>(this);
                return true;
            }
            return false;
        }
    };
}

using Utils::RefCounted;