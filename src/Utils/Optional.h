/**
 * @file Optional.h
 * @author Matěj Bucek (matejbucek)
 */
#pragma once

#include <Utils/Assertions.h>
#include <Utils/Traits.h>
#include <Utils/kmalloc.h>

namespace Utils {
  template<class E>
  class Optional {
    bool m_has_value;
    alignas(E) char m_storage[sizeof(E)];

    E* value_ptr() { return reinterpret_cast<E*>(m_storage); }

    const E* value_ptr() const { return reinterpret_cast<const E*>(m_storage); }

  public:
    Optional() : m_has_value(false) {}

    Optional(const E& value) : m_has_value(true) {
      new(m_storage) E(value);// Placement new
    }

    Optional(E&& value) : m_has_value(true) {
      new(m_storage) E(move(value));// Move construct
    }

    Optional(const Optional& other) : m_has_value(other.m_has_value) {
      if(m_has_value) {
        new(m_storage) E(*other.value_ptr());
      }
    }

    Optional(Optional&& other) noexcept : m_has_value(other.m_has_value) {
      if(m_has_value) {
        new(m_storage) E(move(*other.value_ptr()));
        other.reset();
      }
    }

    Optional& operator=(const Optional& other) {
      if(this != &other) {
        reset();
        if(other.m_has_value) {
          new(m_storage) E(*other.value_ptr());
          m_has_value = true;
        }
      }
      return *this;
    }

    Optional& operator=(Optional&& other) noexcept {
      if(this != &other) {
        reset();
        if(other.m_has_value) {
          new(m_storage) E(move(*other.value_ptr()));
          m_has_value = true;
          other.reset();
        }
      }
      return *this;
    }

    ~Optional() {
      reset();
    }

    bool has_value() const { return m_has_value; }

    E& value() {
      runtime_assert(m_has_value, "Accessing value of an empty Optional!");
      return *value_ptr();
    }

    const E& value() const {
      runtime_assert(m_has_value, "Accessing value of an empty Optional!");
      return *value_ptr();
    }

    void reset() {
      if(m_has_value) {
        value_ptr()->~E();// Manually call destructor
        m_has_value = false;
      }
    }
  };
}// namespace Utils
