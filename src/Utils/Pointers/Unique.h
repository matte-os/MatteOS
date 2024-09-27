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
    Unique(Unique<T>&& other) noexcept : m_value(other.m_value) {
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

    const T* operator->() const {
      return m_value;
    }

    const T& operator*() const {
      return *m_value;
    }

    operator bool() const {
      return m_value != nullptr;
    }

    Unique<T>& operator=(Unique<T> const& ptr) = delete;

    Unique<T>& operator=(Unique<T>&& ptr) noexcept {
      if(this == &ptr)
        return *this;
      delete m_value;
      m_value = ptr.m_value;
      ptr.m_value = nullptr;
      return *this;
    }

    template<typename... Args>
    static Unique<T> make(Args&&... args) {
      return Unique<T>(new T(forward<Args>(args)...));
    }
  };

}// namespace Utils