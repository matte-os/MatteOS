#pragma once

#include <Utils/Pointers/RefPtr.h>
#include <Utils/Strings/StringValue.h>
#include <Utils/Utility.h>

using Utils::Pointers::RefPtr;

namespace Utils::Strings {
  class StringView;
  class String {
    friend class StringView;

  private:
    RefPtr<StringValue> m_value;

  public:
    String() : String("") {}
    String(const String& other) : m_value(StringValue::create(other.m_value->value(), other.m_value->length())) {}
    String(String&& other) noexcept : m_value(move(other.m_value)) {}
    explicit String(const char*);
    ~String() = default;
    size_t length() {
      return m_value->length();
    }
    static u64 calculate_size(const char*);
    String& operator=(const String& other) = default;
    String& operator=(String&& other) noexcept = default;
  };

  class StringView {
  private:
    RefPtr<StringValue> m_value;
    size_t m_offset{};

  public:
    StringView(const String& string) : m_value(string.m_value) {}
    StringView(const char* cstring) : m_value(StringValue::create(cstring, String::calculate_size(cstring))) {}
    StringView offset(size_t offset) {
      StringView view = *this;
      view.m_offset = offset;
      return view;
    }
    [[nodiscard]] const char* value() const { return m_value->value() + m_offset; }
    [[nodiscard]] size_t length() const { return m_value->length() - m_offset; }
    [[nodiscard]] char operator[](size_t index) const { return value()[m_offset + index]; }
  };
};// namespace Utils::Strings