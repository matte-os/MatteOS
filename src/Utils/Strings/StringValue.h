//
// Created by matejbucek on 10.9.22.
//

#pragma once

#include <Utils/Pointers/RefCounted.h>
#include <Utils/Pointers/RefPtr.h>
#include <Utils/Types.h>

using Utils::Pointers::RefPtr;

namespace Utils::Strings {
  class StringValue : public RefCounted<StringValue> {
  private:
    char* m_value{};
    size_t m_length{};

  public:
    static StringValue& empty_string();
    static RefPtr<StringValue> create(const char*, size_t);
    const char* value() const { return m_value; }
    size_t length() const { return m_length; }
    ~StringValue();

  protected:
    StringValue() = default;
  };
}// namespace Utils::Strings