//
// Created by matejbucek on 1.9.22.
//

#pragma once

namespace Utils::Pointers {
  template<typename T>
  class RefPtr {
    T* m_value;

  public:
    RefPtr() : RefPtr(nullptr) {}

    explicit RefPtr(T* ptr) : m_value(ptr) {
      if(m_value) m_value->ref();
    }

    RefPtr(const RefPtr<T>& other) {
      if(m_value) m_value->deref();
      m_value = other.m_value;
      if(m_value) m_value->ref();
    }

    RefPtr(RefPtr<T> const&& other) noexcept {
      if(m_value) m_value->deref();
      m_value = other.m_value;
      if(m_value) m_value->ref();
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
      m_value = ptr.m_value;
      if(m_value)
        m_value->ref();
      return *this;
    }
  };
  template<typename T>
  RefPtr<T> adopt_ref();
}// namespace Utils::Pointers
