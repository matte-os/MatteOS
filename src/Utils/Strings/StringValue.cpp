/**
 * @file StringValue.cpp
 * @author Matěj Bucek (matejbucek)
 *
 * This file was inspired by SerenityOS.
 */
#include <Utils/DebugConsole.h>
#include <Utils/Memory.h>
#include <Utils/Strings/StringValue.h>
#include <Utils/kmalloc.h>
#include <Utils/Assertions.h>

namespace Utils {

  static StringValue* s_empty_string = nullptr;

  StringValue& StringValue::empty_string() {
    if(!s_empty_string) {
      s_empty_string = new StringValue();
      s_empty_string->m_length = 0;
    }
    return *s_empty_string;
  }

  RefPtr<StringValue> StringValue::create(const char* cstring, size_t length) {
    auto* ptr = new StringValue;
    ptr->m_value = new char[length + 1];
    if(length != 0) {
      memcpy(ptr->m_value, cstring, length);
    }
    ptr->m_value[length] = '\0';
    ptr->m_length = length;
    ptr->m_type = ValueType::ASCII;
    return {ptr};
  }

  StringValue::~StringValue() {
    if(m_length != 0) {
      delete[] m_value;
    }
  }

  RefPtr<StringValue> StringValue::adopt(char* cstring, size_t length) {
    auto ptr = RefPtr<StringValue>(new StringValue);
    ptr->m_value = cstring;
    ptr->m_length = length;
    return ptr;
  }

  void StringValue::append(const RefPtr<StringValue>& other) {
    size_t new_length = m_length + other->length();
    char* new_value = new char[new_length + 1];
    memcpy(new_value, m_value, m_length);
    memcpy(new_value + m_length, other->m_value, other->m_length);
    new_value[new_length] = '\0';
    delete[] m_value;
    m_value = new_value;
    m_length = new_length;
  }

  RefPtr<StringValue> StringValue::from_utf16(const u16* string, size_t length) {
    auto* ptr = new StringValue;
    ptr->m_type = ValueType::UTF16;
    ptr->m_value = new char[length * 2 + 2];
    if(length != 0) {
      memcpy(ptr->m_value, reinterpret_cast<const char*>(string), static_cast<int>(length) * 2);
    }
    ptr->m_value[length * 2] = '\0';
    ptr->m_value[length * 2 + 1] = '\0';
    ptr->m_length = length;
    return {ptr};
  }

  const char* StringValue::value() const {
    if(m_type == ValueType::ASCII) {
      return m_value;
    } else {
      return reinterpret_cast<const char*>(m_value);
    }
  }

  char StringValue::operator[](int index) {
    runtime_assert(index >= 0 && index < m_length, "Index out of bounds");
    if(m_type == ValueType::ASCII) {
      return m_value[index];
    } else {
      return m_value[index + 1];
    }
  }

  void StringValue::append(char value) {
    size_t new_length = m_length + 1;
    char* new_value = new char[new_length + 1];
    memcpy(new_value, m_value, m_length);
    new_value[m_length] = value;
    new_value[new_length] = '\0';
    delete[] m_value;
    m_value = new_value;
    m_length = new_length;
  }
}// namespace Utils