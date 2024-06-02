#include <Utils/Strings/String.h>

namespace Utils::Strings {
  String::String(const char* value) {
    this->m_value = StringValue::create(value, calculate_size(value));
  }

  u64 String::calculate_size(const char* cstring) {
    u64 i = 0;
    while(*(cstring + i) != '\0') i++;
    return i;
  }
  String::String(const char* src, size_t length) {
    this->m_value = StringValue::create(src, length);
  }
  String& String::operator=(const String& other) {
    if(this == &other) return *this;
    m_value = StringValue::create(other.m_value->value(), other.m_value->length());
    return *this;
  }
  String& String::operator=(String&& other) noexcept {
    if(this == &other) return *this;
    m_value = move(other.m_value);
    return *this;
  }
};// namespace Utils::Strings