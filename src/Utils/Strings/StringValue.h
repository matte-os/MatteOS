/**
 * @file StringValue.cpp
 * @author Matěj Bucek (matejbucek)
 *
 * This file was inspired by SerenityOS.
 */
#pragma once

#include <Utils/DebugConsole.h>
#include <Utils/Pointers/RefCounted.h>
#include <Utils/Pointers/RefPtr.h>
#include <Utils/Types.h>

using Utils::DebugConsole;
using Utils::RefPtr;

namespace Utils {
  class StringValue : public RefCounted<StringValue> {
  private:
    char* m_value {};
    size_t m_length {};
    enum class ValueType {
      ASCII,
      UTF16
    } m_type {};

  public:
    static StringValue& empty_string();
    static RefPtr<StringValue> create(const char*, size_t);
    static RefPtr<StringValue> adopt(char*, size_t);
    static RefPtr<StringValue> from_utf16(const u16*, size_t);
    const char* value() const;

    size_t length() const { return m_length; }

    char operator[](int index);

    void append(const RefPtr<StringValue>& other);
    void append(char);
    virtual ~StringValue();

  protected:
    StringValue() = default;
  };
}// namespace Utils