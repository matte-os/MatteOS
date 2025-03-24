#pragma once

#include <Utils/DebugConsole.h>
#include <Utils/Forwards.h>
#include <Utils/Pointers/RefPtr.h>
#include <Utils/Strings/StringValue.h>
#include <Utils/Traits.h>

namespace Utils {
  class StringView;

  class String {
    friend class StringView;

    RefPtr<StringValue> m_value;

    explicit String(RefPtr<StringValue> value) : m_value(move(value)) {}

  public:
    String() : String("") {}

    String(const String& other) : m_value(StringValue::create(other.m_value->value(), other.m_value->length())) {}

    String(String&& other) noexcept : m_value(move(other.m_value)) {}

    // ReSharper disable once CppNonExplicitConvertingConstructor
    String(const char*);
    explicit String(const char* src, size_t length);
    ~String() = default;

    [[nodiscard]] size_t length() const {
      return m_value->length();
    }

    [[nodiscard]] const char* to_cstring() const { return m_value->value(); }

    static u64 calculate_size(const char*);
    static String from_int(int value);
    static String from_u64(u64 value);
    static String from_utf16(const u16* utf16, int length = -1);

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

    bool operator==(const StringView& other) const { return equals(other); }

    bool operator!=(const String& other) const { return !equals(other); }

    bool operator!=(const char* other) const { return !equals(other); }

    bool operator!=(const StringView& other) const { return !(*this == other); }

    char operator[](int index) const { return (*m_value)[index]; };

    void operator+=(const String& other);
    void operator+=(char other);
    String operator+(const String& other) const;
    [[nodiscard]] bool starts_with(const String& other) const;

    [[nodiscard]] ArrayList<String> split(const String& separator) const;

    [[nodiscard]] size_t to_uint(size_t base = 10) const;

    static String repeat(const char* string, size_t count);
    [[nodiscard]] String repeat(size_t count) const;

    static String adopt(char* cstring) {
      auto value = StringValue::adopt(cstring, calculate_size(cstring));
      return String(move(value));
    }
  };

  class StringView {
  private:
    RefPtr<StringValue> m_value;
    size_t m_offset {};

  public:
    // ReSharper disable once CppNonExplicitConvertingConstructor
    StringView(const String& string) : m_value(string.m_value) {}

    // ReSharper disable once CppNonExplicitConvertingConstructor
    StringView(const char* cstring) : m_value(StringValue::create(cstring, String::calculate_size(cstring))) {}

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

    operator String() const {
      return String(m_value);
    }

    explicit operator const char*() const {
      return value();
    }
  };

  char to_lower(char c);
  char to_upper(char c);
  bool equals(const char* a, const char* b);
};// namespace Utils