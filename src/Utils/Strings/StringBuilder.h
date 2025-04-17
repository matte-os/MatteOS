/**
 * @file StringBuilder.h
 * @author Matěj Bucek (matejbucek)
 */
#pragma once

#include <Utils/Strings/String.h>

namespace Utils {
  using Utils::String;
  class StringBuilder {
    static constexpr size_t DEFAULT_CAPACITY = 16;
    char* m_data;
    size_t m_capacity;
    size_t m_size ;
    void grow(size_t new_capacity);
  public:
    StringBuilder() : m_data(new char[DEFAULT_CAPACITY]), m_capacity(DEFAULT_CAPACITY), m_size(0) {};
    StringBuilder(const StringBuilder& other);
    StringBuilder(StringBuilder&& other) noexcept;

    StringBuilder& operator=(const StringBuilder& other);
    StringBuilder& operator=(StringBuilder&& other) noexcept;

    void operator+=(const char* str);
    void operator+=(const char c);
    void operator+=(const String& str);

    String to_string() const {
      return String(m_data);
    }

    ~StringBuilder();
  };

}// namespace Utikls
