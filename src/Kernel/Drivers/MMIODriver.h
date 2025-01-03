#pragma once

#include <Kernel/Drivers/Driver.h>


namespace Kernel {
  class MMIODriver : public Driver {
  public:
    void init() override;
    void shutdown() override;
  };

  class MMIODriverDescriptor : public DriverDescriptor {
  public:
    ErrorOr<RefPtr<Driver>> instantiate_driver() override;
  };
}


