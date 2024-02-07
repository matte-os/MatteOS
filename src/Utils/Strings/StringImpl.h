//
// Created by matejbucek on 10.9.22.
//

#pragma once

#include <Utils/Pointers/RefCounted.h>
#include <Utils/Pointers/RefPtr.h>
#include <Utils/Types.hh>

using Utils::Pointers::RefPtr;

namespace Utils::Strings {
    class String;
    class StringImpl : public RefCounted<StringImpl> {
        friend class String;
    private:
        char* m_value;
        size_t m_length;
    public:
        static StringImpl& empty_string();
        static RefPtr<StringImpl> create(const char*, size_t);
        ~StringImpl();
    protected:
        StringImpl() = default;
    };
}