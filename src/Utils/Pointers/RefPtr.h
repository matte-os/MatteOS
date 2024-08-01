//
// Created by matejbucek on 1.9.22.
//

#pragma once
#include <Utils/Traits.h>

namespace Utils {
  template<typename T>
  class RefPtr {
    T* m_value;

  public:
    RefPtr() : RefPtr(nullptr) {}

    explicit RefPtr(T* ptr) : m_value(ptr) {
      if(m_value) m_value->ref();
    }

    RefPtr(const RefPtr<T>& other) : m_value(other.m_value) {
      if(m_value) m_value->ref();
    }

    RefPtr(RefPtr<T>&& other) noexcept : m_value(other.m_value) {
      other.m_value = nullptr;
    }

    ~RefPtr() {
      if(m_value)
        m_value->unref();
      m_value = nullptr;
    }

    T* ptr() const {
      return m_value;
    }

    T* operator->() const {
      return m_value;
    }

    RefPtr<T>& operator=(RefPtr<T> const& ptr) {
      if(this == &ptr)
        return *this;
      if(m_value)
        m_value->unref();
      m_value = ptr.m_value;
      if(m_value)
        m_value->ref();
      return *this;
    }

    RefPtr<T>& operator=(RefPtr<T>&& ptr) noexcept {
      if(this == &ptr)
        return *this;
      if(m_value)
        m_value->unref();
      m_value = ptr.m_value;
      ptr.m_value = nullptr;
      return *this;
    }

    bool operator==(const RefPtr<T>& other) const {
      return m_value == other.m_value;
    }

    bool operator!=(const RefPtr<T>& other) const {
      return m_value != other.m_value;
    }

    bool operator==(const T* other) const {
      return m_value == other;
    }

    bool operator!=(const T* other) const {
      return m_value != other;
    }

    operator bool() const {
      return m_value != nullptr;
    }

    template<typename U>
    operator RefPtr<U>() const {
      static_assert(IsBaseOf<U, T>::value, "Cannot convert to a non-base class!");
      return RefPtr<U>(static_cast<U*>(m_value));
    }
  };
  template<typename T>
  RefPtr<T> adopt_ref();
}// namespace Utils
