#pragma once

#include <Kernel/Drivers/DeviceDriver.h>
#include <Kernel/Forwards.h>

namespace Kernel {

  class BlockDeviceDriver : public DeviceDriver {
  public:
    virtual ErrorOr<void> write_async(s64 request_id, u8* buffer, u64 size, u64 offset) = 0;
    virtual ErrorOr<void> read_async(s64 request_id, u8* buffer, u64 size, u64 offset) = 0;
    virtual ErrorOr<void> write_poll(u8* buffer, u64 size, u64 offset) = 0;
    virtual ErrorOr<void> read_poll(u8* buffer, u64 size, u64 offset) = 0;
    virtual ErrorOr<ArrayList<RefPtr<BlockDeviceAsyncResult>>> notify() = 0;
  };

}// namespace Kernel
