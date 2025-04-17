/**
 * @file ConsoleDeviceDriver.h
 * @author Matěj Bucek
 */
#pragma once

#include <Kernel/Drivers/DeviceDriver.h>
#include <Utils/Strings/String.h>

namespace Kernel {
  using Utils::String;

  class ConsoleDeviceDriver : public DeviceDriver {
  public:
    virtual void write(char c) = 0;
    virtual void write(String& message) = 0;
    virtual ErrorOr<String> read_line() = 0;
    virtual bool line_available() = 0;

    virtual void handle_interrupt(u64 interrupt_id) = 0;
  };
}// namespace Kernel
