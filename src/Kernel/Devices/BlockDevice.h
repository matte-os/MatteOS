#pragma once

#include <Kernel/Devices/Device.h>

namespace Kernel {

  class BlockDevice : public Device {
  public:
    explicit BlockDevice(RefPtr<UnderlyingDevice> underlying_device, ArrayList<u64>&& interrupts) : Device(move(underlying_device), move(interrupts), DeviceType::Block, true) {}

    ErrorOr<void> write(u8* buffer, u64 size, u64 offset);
    ErrorOr<void> read(u8* buffer, u64 size, u64 offset);
    ErrorOr<void> write_poll(u8* buffer, u64 size, u64 offset);
    ErrorOr<void> read_poll(u8* buffer, u64 size, u64 offset);
    ErrorOr<void> init() override;

    ErrorOr<void> handle_interrupt(u64 interrupt_id) override;
  };

}// namespace Kernel
