/**
 * @file StringBuilder.cpp
 * @author Matěj Bucek (matejbucek)
 */
#include <Utils/Memory.h>
#include <Utils/Strings/StringBuilder.h>

namespace Utils {
  void StringBuilder::grow(size_t new_capacity) {
    new_capacity += DEFAULT_CAPACITY;
    auto new_data = new char[new_capacity];
    memcpy(new_data, m_data, m_size);
    delete[] m_data;
    m_data = new_data;
    m_capacity = new_capacity;
  }

  void StringBuilder::operator+=(const char* str) {
    auto length = String::calculate_size(str);
    if(m_size + length >= m_capacity) {
      grow(m_capacity + length);
    }

    memcpy(m_data + m_size, str, length);
  }

  void StringBuilder::operator+=(const char c) {
    if(m_size + 1 >= m_capacity) {
      grow(m_capacity + 1);
    }

    m_data[m_size] = c;
    m_size++;
  }

  void StringBuilder::operator+=(const String& str) {
    *this += str.to_cstring();
  }

  StringBuilder::~StringBuilder() {
    delete[] m_data;
  }

  StringBuilder::StringBuilder(const StringBuilder& other) {
    m_data = new char[other.m_capacity];
    m_capacity = other.m_capacity;
    m_size = other.m_size;
    memcpy(m_data, other.m_data, m_size);
  }
  StringBuilder::StringBuilder(StringBuilder&& other) noexcept {
    m_data = other.m_data;
    m_capacity = other.m_capacity;
    m_size = other.m_size;
    other.m_data = nullptr;
    other.m_capacity = 0;
    other.m_size = 0;
  }
  StringBuilder& StringBuilder::operator=(const StringBuilder& other) {
    if(this == &other)
      return *this;
    delete[] m_data;
    m_data = new char[other.m_capacity];
    m_capacity = other.m_capacity;
    m_size = other.m_size;
    memcpy(m_data, other.m_data, m_size);
    return *this;
  }
  StringBuilder& StringBuilder::operator=(StringBuilder&& other) noexcept {
    if(this == &other)
      return *this;
    delete[] m_data;
    m_data = other.m_data;
    m_capacity = other.m_capacity;
    m_size = other.m_size;
    other.m_data = nullptr;
    other.m_capacity = 0;
    other.m_size = 0;
    return *this;
  }
}// namespace Utils