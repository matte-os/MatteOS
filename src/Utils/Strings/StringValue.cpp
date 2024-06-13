//
// Created by matejbucek on 10.9.22.
//

#include <Utils/DebugConsole.h>
#include <Utils/Memory.h>
#include <Utils/Strings/StringValue.h>
#include <Utils/kmalloc.h>

namespace Utils::Strings {

  static StringValue* s_empty_string = nullptr;

  StringValue& StringValue::empty_string() {
    if(!s_empty_string) {
      s_empty_string = new StringValue();
      s_empty_string->m_length = 0;
    }
    return *s_empty_string;
  }





















































  RefPtr<StringValue> StringValue::create(const char* cstring, size_t t_length) {
    auto* ptr = new StringValue;
    ptr->m_value = new char[t_length + 1];
    memcpy(ptr->m_value, cstring, t_length);
    ptr->m_value[t_length] = '\0';
    ptr->m_length = t_length;
    return RefPtr<StringValue>(ptr);
  }

  StringValue::~StringValue() {
    if(m_length != 0) {
      delete[] m_value;
    }
  }
}// namespace Utils::Strings