/**
 * @file Unique.h
 *
 * @brief A unique smart pointer.
 *
 * @author Matěj Bucek
 */
#pragma once

#include <Utils/Traits.h>

namespace Utils {
  template<typename T>
  class Unique {
  private:
    T* m_value;
    Unique(T* value) : m_value(value) {}

  public:
    Unique(Unique<T>&& other) : m_value(other.m_value) {
      other.m_value = nullptr;
    }

    explicit Unique(const Unique<T>& other) = delete;

    ~Unique() {
      delete m_value;
      m_value = nullptr;
    }

    T* operator->() {
      return m_value;
    }

    T& operator*() {
      return *m_value;
    }

    template<typename... Args>
    static Unique<T> make(Args&&... args) {
      return Unique<T>(new T(forward<Args>(args)...));
    }
  };

}// namespace Utils