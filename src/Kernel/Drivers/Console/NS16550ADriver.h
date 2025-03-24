//
// Created by matejbucek on 1.1.25.
//

#pragma once

#include <Kernel/Devices/Console/ConsoleDevice.h>
#include <Kernel/Drivers/Console/ConsoleDeviceDriver.h>
#include <Kernel/Drivers/DeviceDriver.h>
#include <Utils/Arrays/Queue.h>

namespace Kernel {
  using Utils::LinkedQueue;

  class NS16550ADriver final : public ConsoleDeviceDriver {
    RefPtr<ConsoleDevice> m_device;
    String m_buffer;
    Utils::LinkedQueue<String> m_lines;

  public:
    void init(RefPtr<Device> device) override;
    void shutdown() override;
    void write(char c) override;
    void write(String& message) override;
    ErrorOr<String> read_line() override;
    ErrorOr<char> read_char();

    void handle_interrupt(u64 interrupt_id) override;
  };

  class NS16550ADriverDescriptor final : public DeviceDriverDescriptor {
  public:
    bool is_compatible_with(RefPtr<Kernel::Device> device) override;
    ErrorOr<RefPtr<Kernel::DeviceDriver>> instantiate_driver(RefPtr<Kernel::Device> device) override;
  };

}// namespace Kernel
