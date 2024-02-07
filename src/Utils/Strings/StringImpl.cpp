//
// Created by matejbucek on 10.9.22.
//

#include <Utils/Strings/StringImpl.h>
#include <Utils/kmalloc.h>
#include <Utils/Memory.hh>
#include <Utils/DebugConsole.hh>

namespace Utils::Strings {

    static StringImpl* s_empty_string = nullptr;

    StringImpl& StringImpl::empty_string() {
        if(!s_empty_string) {
            s_empty_string = new StringImpl();
            s_empty_string->m_length = 0;
        }
        return *s_empty_string;
    }

    RefPtr<StringImpl> StringImpl::create(const char * cstring, size_t t_length) {
        auto* ptr = new StringImpl;
        ptr->m_value = new char[t_length];
        memcpy(ptr->m_value, cstring, t_length);
        ptr->m_length = t_length;
        return RefPtr<StringImpl>(ptr);
    }

    StringImpl::~StringImpl() {
        if(m_length != 0) {
            delete[] m_value;
        }
    }
}