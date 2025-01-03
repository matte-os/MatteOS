#pragma once

#include <Kernel/Devices//Device.h>

namespace Kernel {

  class ConsoleDevice : public Device {
  public:
    explicit ConsoleDevice(RefPtr<UnderlyingDevice> underlying_device, ArrayList<u64>&& interrupts) : Device(move(underlying_device), move(interrupts), DeviceType::Console, true) {}

    ErrorOr<void> init() override;
    ErrorOr<void> write(char c);
    ErrorOr<void> write(String message);
    ErrorOr<String> read();
    ErrorOr<char> read_char();
    ErrorOr<void> handle_interrupt(u64 interrupt_id) override;

    bool supports_read_string() const;
  };

}// namespace Kernel
