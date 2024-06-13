#pragma once

#include <Utils/Pointers/RefPtr.h>
#include <Utils/Strings/StringValue.h>
#include <Utils/Utility.h>
#include <Utils/DebugConsole.h>

using Utils::Pointers::RefPtr;
using Utils::DebugConsole;

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
    explicit String(const char* src, size_t length);
    ~String() = default;
    size_t length() {
      return m_value->length();
    }
    [[nodiscard]] const char* to_cstring() const { return m_value->value(); }
    static u64 calculate_size(const char*);
    String& operator=(const String& other);
    String& operator=(String&& other) noexcept;
    [[nodiscard]] bool equals(const String& other) const;
    bool equals(const char* other) const;
    [[nodiscard]] bool equals(const StringView& other) const;
    [[nodiscard]] bool equals_ignore_case(const String& other) const;
    bool equals_ignore_case(const char* other) const;
    [[nodiscard]] bool equals_ignore_case(const StringView& other) const;
    bool operator==(const String& other) const { return equals(other); }
    bool operator==(const char* other) const { return equals(other); }
    bool operator==(const StringView& other) const { return equals(other);}
    bool operator!=(const String& other) const { return !equals(other); }
    bool operator!=(const char* other) const { return !equals(other); }
    bool operator!=(const StringView& other) const { return !(*this == other); }
  };

  class StringView {
  private:
    RefPtr<StringValue> m_value;
    size_t m_offset{};

  public:
    explicit StringView(const String& string) : m_value(string.m_value) {}
    explicit StringView(const char* cstring) : m_value(StringValue::create(cstring, String::calculate_size(cstring))) {}
    StringView offset(size_t offset) {
      StringView view = *this;
      view.m_offset = offset;
      return view;
    }
    [[nodiscard]] const char* value() const { return m_value->value() + m_offset; }
    [[nodiscard]] size_t length() const { return m_value->length() - m_offset; }
    [[nodiscard]] char operator[](size_t index) const { return value()[m_offset + index]; }
    bool equals(const char* other) const;
    [[nodiscard]] bool equals(const String& other) const;
    [[nodiscard]] bool equals(const StringView& other) const;
    bool equals_ignore_case(const char* other) const;
    [[nodiscard]] bool equals_ignore_case(const String& other) const;
    [[nodiscard]] bool equals_ignore_case(const StringView& other) const;
    bool operator==(const String& other) const;
    bool operator!=(const String& other) const { return !(*this == other); }
    bool operator==(const char* other) const;
    bool operator!=(const char* other) const { return !(*this == other); }
    bool operator==(const StringView& other) const;
    bool operator!=(const StringView& other) const { return !(*this == other); }
  };

  char to_lower(char c);
  char to_upper(char c);
  bool equals(const char* a, const char* b);
};// namespace Utils::Strings