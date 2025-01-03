#pragma once

#include <Kernel/Drivers/DeviceDriver.h>
#include <Utils/Strings/String.h>

namespace Kernel {
  using Utils::String;

  class ConsoleDeviceDriver : public DeviceDriver {
  public:
    virtual void write(char c) = 0;
    virtual void write(String& message) = 0;
    virtual ErrorOr<String> read() = 0;
    virtual ErrorOr<char> read_char() = 0;
    virtual bool supports_read_string() = 0;

    virtual void handle_interrupt(u64 interrupt_id) = 0;
  };
}// namespace Kernel
