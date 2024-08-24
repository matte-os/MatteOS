//
// Created by matejbucek on 24.8.24.
//

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
    virtual void init(RefPtr<Device> device) = 0;
    virtual void shutdown() = 0;

    template<typename T>
    T* as() {
      return static_cast<T*>(this);
    }
  };

  class DriverDescriptor : public RefCounted<DriverDescriptor> {
  public:
    virtual bool is_compatible_with(RefPtr<Device> device) = 0;
    virtual ErrorOr<RefPtr<Driver>> instantiate_driver(RefPtr<Device> device) = 0;
  };

}// namespace Kernel
