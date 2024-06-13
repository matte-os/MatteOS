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
  bool String::equals(const String& other) const {
    return equals(other.to_cstring());
  }
  bool String::equals(const char* other) const {
    if(m_value->length() != calculate_size(other)) return false;
    for(size_t i = 0; i < m_value->length(); i++) {
      if(m_value->value()[i] != other[i]) return false;
    }
    return true;
  }
  bool String::equals_ignore_case(const String& other) const {
    return equals_ignore_case(other.to_cstring());
  }
  bool String::equals_ignore_case(const char* other) const {
    if(m_value->length() != calculate_size(other)) return false;
    for(size_t i = 0; i < m_value->length(); i++) {
      if(to_lower(m_value->value()[i]) != to_lower(other[i])) return false;
    }
    return true;
  }
  bool String::equals(const StringView& other) const {
    return equals(other.value());
  }
  bool String::equals_ignore_case(const StringView& other) const {
    return equals_ignore_case(other.value());
  }
  char to_lower(char c) {
    if(c >= 'A' && c <= 'Z') return c + 32;
    return c;
  }
  char to_upper(char c) {
    if(c >= 'a' && c <= 'z') return c - 32;
    return c;
  }
  bool StringView::equals(const char* other) const {
    if(m_value->length() != String::calculate_size(other)) return false;
    for(size_t i = 0; i < m_value->length(); i++) {
      if(m_value->value()[i] != other[i]) return false;
    }
    return true;
  }
  bool StringView::equals(const String& other) const {
    return equals(other.to_cstring());
  }
  bool StringView::equals(const StringView& other) const {
    return equals(other.value());
  }
  bool StringView::equals_ignore_case(const char* other) const {
    if(m_value->length() != String::calculate_size(other)) return false;
    for(size_t i = 0; i < m_value->length(); i++) {
      if(to_lower(m_value->value()[i]) != to_lower(other[i])) return false;
    }
    return true;
  }
  bool StringView::equals_ignore_case(const StringView& other) const {
    return equals_ignore_case(other.value());
  }
  bool StringView::equals_ignore_case(const String& other) const {
    return equals_ignore_case(other.to_cstring());
  }
  bool StringView::operator==(const String& other) const {
    return equals(other);
  }
  bool StringView::operator==(const char* other) const {
    return equals(other);
  }
  bool StringView::operator==(const StringView& other) const {
    return equals(other);
  }
};// namespace Utils::Strings