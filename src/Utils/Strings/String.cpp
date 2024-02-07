#include <Utils/Strings/String.h>

namespace Utils::Strings {

    String::String(const char * value) {
        this->m_value = StringImpl::create(value, calculate_size(value));
    }

    u64 String::calculate_size(const char * cstring) {
        u64 i = 0;
        while(*(cstring + i) != '\0') i++;
        return i;
    }
};