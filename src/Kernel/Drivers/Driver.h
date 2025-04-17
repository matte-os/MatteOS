/**
 * @file Driver.h
 * @author Matěj Bucek
 */
#pragma once

#include <Kernel/Forwards.h>
#include <Utils/Errors/ErrorOr.h>
#include <Utils/Pointers/RefCounted.h>
#include <Utils/Pointers/RefPtr.h>

namespace Kernel {

  using Utils::ErrorOr;
  using Utils::RefCounted;
  using Utils::RefPtr;

  class Driver : public RefCounted<Driver> {
  public:
    virtual void init() = 0;
    virtual void shutdown() = 0;

    template<typename T>
    T* as() {
      return static_cast<T*>(this);
    }
  };

  class DriverDescriptor : public RefCounted<DriverDescriptor> {
  public:
    virtual ErrorOr<RefPtr<Driver>> instantiate_driver() = 0;
  };

}// namespace Kernel
