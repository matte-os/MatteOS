//
// Created by matejbucek on 1.9.22.
//

#pragma once
#include <Utils/DebugConsole.hh>

namespace Utils::Pointers {
    template <typename T> class RefPtr {
        T* value;
    public:
        RefPtr() : RefPtr(nullptr) {}

        explicit RefPtr(T* ptr) : value(ptr) {
            if(value) value->ref();
        }

        ~RefPtr() {
            if(value)
                value->unref();
            value = nullptr;
        }

        T* getValue() const {
            return value;
        }

        T* operator->() {
            return value;
        }

        RefPtr<T>& operator=(RefPtr<T> const& ptr){
            value = ptr.value;
            if(value)
                value->ref();
            return *this;
        }
    };
} // Utils
