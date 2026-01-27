/**
 * @file RefCounted.h
 * @author Matěj Bucek (matejbucek)
 *
 * This file was inspired by SerenityOS.
 */
#pragma once

#include <Utils/kmalloc.h>

namespace Utils {
  class RefCountedBase {
  public:
    unsigned int mutable m_ref_count = 0;

    void ref() const {
      ++m_ref_count;
    }

    unsigned int deref() const {
      return --m_ref_count;
    }

  protected:
    RefCountedBase() = default;
    virtual ~RefCountedBase() = default;
  };

  template<typename T>
  class RefCounted : public RefCountedBase {
  public:
    virtual ~RefCounted() = default;

    bool unref() const {
      if(deref() == 0) {
        delete static_cast<T const*>(this);
        return true;
      }
      return false;
    }
  };
}// namespace Utils

using Utils::RefCounted;