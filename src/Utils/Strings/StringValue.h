//
// Created by matejbucek on 10.9.22.
//

#pragma once

#include <Utils/Pointers/RefCounted.h>
#include <Utils/Pointers/RefPtr.h>
#include <Utils/Types.h>
#include <Utils/DebugConsole.h>

using Utils::Pointers::RefPtr;
using Utils::DebugConsole;

namespace Utils::Strings {
  class StringValue : public RefCounted<StringValue> {
  private:
    char* m_value{};
    size_t m_length{};

  public:
    static StringValue& empty_string();
    static RefPtr<StringValue> create(const char*, size_t);
    static RefPtr<StringValue> adopt(char*, size_t);
    const char* value() const { return m_value; }
    size_t length() const { return m_length; }
    char operator[](int index) const { return m_value[index]; }
    void append(const RefPtr<StringValue>& other);
    virtual ~StringValue();

  protected:
    StringValue() = default;
  };
}// namespace Utils::Strings