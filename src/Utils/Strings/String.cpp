#include <Utils/Strings/String.h>

namespace Utils::Strings {

    String::String(const char * value) {
        this->value = StringImpl::create(value, calculateSize(value));
    }

    u64 String::calculateSize(const char * cstring) {
        u64 i = 0;
        while(*(cstring + i) != '\0') i++;
        return i;
    }
};