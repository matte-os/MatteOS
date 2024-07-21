/**
 * @file Shared.h
 *
 * @brief A implicitly reference counted smart pointer.
 *
 * @author Matěj Bucek
 */
#pragma once

#include <Utils/Traits.h>

namespace Utils {
  template<typename T>
  class Value {
  private:
    T* m_value;
    int m_number_of_refs;

  public:
    explicit Value(T* value) : m_value(value), m_number_of_refs(1) {}
    auto ref() {
      return ++m_number_of_refs;
    }
    auto deref() {
      if(--m_number_of_refs == 0) {
        delete m_value;
        m_value = nullptr;
      }
      return m_number_of_refs;
    }
    T* get() {
      return m_value;
    }
  };

  template<typename T>
  class Shared {
  private:
    Value<T>* m_value;
    Shared(Value<T>* value) : m_value(value) {}

  public:
    Shared(const Shared<T>& other) : m_value(other.m_value) {
      if(m_value) {
        m_value->ref();
      }
    }

    Shared(Shared<T>&& other) : m_value(other.m_value) {
      if(m_value) {
        m_value->ref();
      }
    }
    ~Shared() {
      if(m_value) {
        if(!m_value->deref()) {
          delete m_value;
          m_value = nullptr;
        }
      }
    }

    T* operator->() {
      return m_value->get();
    }

    T& operator*() {
      return *m_value->get();
    }

    template<typename... Args>
    static Shared<T> make(Args... args) {
      auto* value = new Value(new T(forward<Args>(args)...));
      return Shared<T>(value);
    }
    //static Shared<T> adopt(T*);
  };
}// namespace Utils
