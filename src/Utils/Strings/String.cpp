#include <Utils/Arrays/ArrayList.h>
#include <Utils/Basic.h>
#include <Utils/Strings/String.h>

namespace Utils {
  String::String(const char* value) : m_value(StringValue::create(value, calculate_size(value))) {}

  u64 String::calculate_size(const char* cstring) {
    u64 i = 0;
    while(*(cstring + i) != '\0') i++;
    return i;
  }

  String::String(const char* src, size_t length) : m_value(StringValue::create(src, length)) {}

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

  String String::repeat(const char* string, size_t count) {
    if(count == 0) return {};

    size_t string_length = calculate_size(string);
    size_t length = string_length * count;
    char* buffer = new char[length + 1];

    for(size_t i = 0; i < count; i++) {
      for(size_t j = 0; j < string_length; j++) {
        buffer[i * string_length + j] = string[j];
      }
    }
    buffer[length] = '\0';
    return adopt(buffer);
  }

  String String::repeat(size_t count) const {
    return String::repeat(m_value->value(), count);
  }

  ArrayList<String> String::split(const String& separator) const {
    ArrayList<String> result;
    size_t last_index = 0;
    for(size_t i = 0; i < m_value->length(); i++) {
      if(m_value->value()[i] == separator[0]) {
        if(i == 0 || i == m_value->length() - 1) {
          last_index = i + 1;
          continue;
        }
        bool found = true;
        for(size_t j = i + 1; j < m_value->length() && separator.length() > 1; j++) {
          if(m_value->value()[j] != separator[j - i]) {
            found = false;
            break;
          }
        }
        if(found) {
          result.add(String(m_value->value() + last_index, i - last_index));
          last_index = i + separator.length();
        }
      }
    }

    if(last_index < m_value->length()) {
      result.add(String(m_value->value() + last_index, m_value->length() - last_index));
    }

    return result;
  }

  bool String::starts_with(const String& other) const {
    if(other.length() > m_value->length()) return false;
    for(size_t i = 0; i < other.length(); i++) {
      if(m_value->value()[i] != other[i]) return false;
    }
    return true;
  }

  size_t String::to_uint(size_t base) const {
    return atoi(m_value->value(), base);
  }

  void String::operator+=(const String& other) {
    m_value->append(other.m_value);
  }

  String String::from_int(int value) {
    //FIXME: We should calculate the largest possible number of digits
    char* buffer = new char[20];
    itoa(buffer, value, 10);
    return adopt(buffer);
  }

  String String::from_u64(u64 value) {
    char* buffer = new char[22];
    itoa(buffer, value, 10);
    return adopt(buffer);
  }

  String String::from_utf16(const u16* utf16, int length) {
    if(length == -1) {
      length = 0;
      while(utf16[length] != 0) length++;
    }
    return String(StringValue::from_utf16(utf16, length));
  }

  void String::operator+=(char other) {
    m_value->append(other);
  }

  String String::operator+(const String& other) const {
    String result;
    result += *this;
    result += other;
    return result;
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
};// namespace Utils