#pragma once

#include <Kernel/Devices//Device.h>

namespace Kernel {

  class ConsoleDevice : public Device {
  public:
    explicit ConsoleDevice(RefPtr<UnderlyingDevice> underlying_device, ArrayList<u64>&& interrupts) : Device(move(underlying_device), move(interrupts), DeviceType::Console) {}

    ErrorOr<void> init() override;
    ErrorOr<void> write(String message);
    ErrorOr<String> read();
    ErrorOr<void> handle_interrupt(u64 interrupt_id) override;
  };

}// namespace Kernel
