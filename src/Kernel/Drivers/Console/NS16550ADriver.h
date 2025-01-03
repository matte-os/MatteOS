//
// Created by matejbucek on 1.1.25.
//

#pragma once

#include <Kernel/Devices/Console/ConsoleDevice.h>
#include <Kernel/Drivers/Console/ConsoleDeviceDriver.h>
#include <Kernel/Drivers/DeviceDriver.h>

namespace Kernel {

  class NS16550ADriver : public ConsoleDeviceDriver {
  private:
    RefPtr<ConsoleDevice> m_device;
  public:
    void init(RefPtr<Kernel::Device> device) override;
    void shutdown() override;
    void write(char c) override;
    void write(String& message) override;
    ErrorOr<String> read() override;
    ErrorOr<char> read_char() override;
    bool supports_read_string() override { return false; }

    void handle_interrupt(u64 interrupt_id) override;
  };

  class NS16550ADriverDescriptor : public DeviceDriverDescriptor {
  public:
    bool is_compatible_with(RefPtr<Kernel::Device> device) override;
    ErrorOr<RefPtr<Kernel::DeviceDriver>> instantiate_driver(RefPtr<Kernel::Device> device) override;
  };

}// namespace Kernel
