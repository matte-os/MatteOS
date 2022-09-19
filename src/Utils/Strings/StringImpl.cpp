//
// Created by matejbucek on 10.9.22.
//

#include <Utils/Strings/StringImpl.h>
#include <Utils/kmalloc.h>
#include <Utils/Memory.hh>
#include <Utils/DebugConsole.hh>

namespace Utils::Strings {

    static StringImpl* emptyStringPtr = nullptr;

    StringImpl& StringImpl::emptyString() {
        if(!emptyStringPtr) {
            emptyStringPtr = new StringImpl();
            emptyStringPtr->length = 0;
        }
        return *emptyStringPtr;
    }

    RefPtr<StringImpl> StringImpl::create(const char * cstring, size_t t_length) {
        auto* ptr = new StringImpl;
        ptr->value = new char[t_length];
        memcpy(ptr->value, cstring, t_length);
        ptr->length = t_length;
        return RefPtr<StringImpl>(ptr);
    }

    StringImpl::~StringImpl() {
        if(length != 0) {
            delete[] value;
        }
    }
}