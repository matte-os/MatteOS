/**
 * @file BlockIODriver.cpp
 * @author Matěj Bucek
 */
#pragma once

#include <Kernel/Drivers/DeviceDriver.h>
#include <Kernel/Drivers/Storage/BlockDeviceDriver.h>
#include <Kernel/Drivers/VirtIO/BlockIO.h>
#include <Utils/Errors/ErrorOr.h>

namespace Kernel {
  using Utils::ErrorOr;

  class BlockIODriver : public BlockDeviceDriver {
    RefPtr<BlockDevice> m_device;
    ArrayList<BlockIO::Request*> m_ignored_requests;// This is used to ignore requests, that were issued using poll methods

  public:
    explicit BlockIODriver(RefPtr<BlockDevice> device);
    ErrorOr<void> write_async(s64 request_id, u8* buffer, u64 size, u64 offset) override;
    ErrorOr<void> read_async(s64 request_id, u8* buffer, u64 size, u64 offset) override;
    ErrorOr<void> write_poll(u8* buffer, u64 size, u64 offset) override;
    ErrorOr<void> read_poll(u8* buffer, u64 size, u64 offset) override;
    ErrorOr<ArrayList<RefPtr<BlockDeviceAsyncResult>>> notify() override;
    void init(RefPtr<Device> device) override;
    void shutdown() override;

  private:
    ErrorOr<BlockIO::Request*> block_operation(s64 request_id, u8* buffer, u64 size, u64 offset, bool is_write);
    RefPtr<VirtIODevice> virtio_device() const;
    const u8 status_value = 0x111;
  };

  class VirtIODeviceDriverDescriptor : public DeviceDriverDescriptor {
  public:
    bool is_compatible_with(RefPtr<Device> device) override;
    ErrorOr<RefPtr<DeviceDriver>> instantiate_driver(RefPtr<Device> device) override;
  };

}// namespace Kernel
