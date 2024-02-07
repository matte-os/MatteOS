#pragma once

#include <Utils/Pointers/RefPtr.h>
#include <Utils/Strings/StringImpl.h>
#include <Utils/DebugConsole.hh>

using Utils::Pointers::RefPtr;

namespace Utils::Strings {
    class String {
    private:
        RefPtr<StringImpl> m_value;
    public:
        String() : String("") {}
        String(const char *);
        ~String() = default;
        size_t length() {
            return m_value->m_length;
        }
        static u64 calculate_size(const char*);
        void operator+=(char* value);
        void operator+=(String& other);
        String operator=(const char* value);
    };
};