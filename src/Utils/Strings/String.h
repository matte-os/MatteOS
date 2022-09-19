#pragma once

#include <Utils/Pointers/RefPtr.h>
#include <Utils/Strings/StringImpl.h>
#include <Utils/DebugConsole.hh>

using Utils::Pointers::RefPtr;

namespace Utils::Strings {
    class String {
    private:
        RefPtr<StringImpl> value;
    public:
        String() : String("") {}
        String(const char *);
        ~String() = default;
        size_t length() {
            return value->length;
        }
        static u64 calculateSize(const char*);
        void operator+=(char* value);
        void operator+=(String& other);
        String operator=(const char* value);
    };
};