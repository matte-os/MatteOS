//
// Created by matejbucek on 24.8.24.
//

#pragma once

#include <Kernel/Drivers/DeviceDriver.h>
#include <Kernel/Drivers/VirtIO/BlockIO.h>
#include <Utils/Errors/ErrorOr.h>

namespace Kernel {
  using Utils::ErrorOr;

  class BlockIODriver : public DeviceDriver {
    RefPtr<BlockDevice> m_device;

  public:
    explicit BlockIODriver(RefPtr<BlockDevice> device);
    ErrorOr<void> write(u8* buffer, u64 size, u64 offset);
    ErrorOr<void> read(u8* buffer, u64 size, u64 offset);
    ErrorOr<void> write_poll(u8* buffer, u64 size, u64 offset);
    ErrorOr<void> read_poll(u8* buffer, u64 size, u64 offset);
    void init(RefPtr<Device> device) override;
    void shutdown() override;

  private:
    ErrorOr<BlockIO::Request*> block_operation(u8* buffer, u64 size, u64 offset, bool is_write);
    RefPtr<VirtIODevice> virtio_device() const;
    const u8 status_value = 0x111;
  };

  class VirtIODeviceDriverDescriptor : public DeviceDriverDescriptor {
  public:
    bool is_compatible_with(RefPtr<Device> device) override;
    ErrorOr<RefPtr<DeviceDriver>> instantiate_driver(RefPtr<Device> device) override;
  };

}// namespace Kernel
