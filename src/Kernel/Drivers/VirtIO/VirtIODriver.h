/**
 * @file VirtIODriver.h
 * @author Matěj Bucek
 */
#pragma once

#include <Kernel/Drivers/Driver.h>

namespace Kernel {

  class VirtIODriver : public Driver {
  public:
    void init() override;
    void shutdown() override;
  };

  class VirtIODriverDescriptor : public DriverDescriptor {
  public:
    ErrorOr<RefPtr<Driver>> instantiate_driver() override {
      return ErrorOr<RefPtr<Driver>>::create(RefPtr<Driver>(new VirtIODriver()));
    }
  };

}// namespace Kernel
