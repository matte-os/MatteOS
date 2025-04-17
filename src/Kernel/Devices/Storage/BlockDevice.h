/**
 * @file BlockDevice.h
 * @author Matěj Bucek
 */
#pragma once

#include <Kernel/Devices/AsyncRequest.h>
#include <Kernel/Devices/Device.h>

namespace Kernel {

  using Utils::Callable;

  enum class BlockDeviceAsyncRequestType {
    Ok,
    Failed
  };

  class BlockDeviceAsyncResult : public AsyncResult {
    BlockDeviceAsyncRequestType m_type;

  public:
    BlockDeviceAsyncResult(s64 request_id, BlockDeviceAsyncRequestType type) : AsyncResult(request_id), m_type(type) {}

    BlockDeviceAsyncRequestType type() const { return m_type; }
  };

  class BlockDeviceAsyncRequest : public AsyncRequest {
    u8* m_buffer;
    u64 m_size;
    u64 m_offset;
    bool m_write;
    Callable<void, RefPtr<BlockDeviceAsyncRequest>> m_on_success;
    Callable<void, RefPtr<BlockDeviceAsyncRequest>> m_on_failure;

  public:
    BlockDeviceAsyncRequest(s64 id, u8* buffer, u64 size, u64 offset, bool write, Callable<void, RefPtr<BlockDeviceAsyncRequest>> on_success, Callable<void, RefPtr<BlockDeviceAsyncRequest>> on_failure) : AsyncRequest(id), m_buffer(buffer), m_size(size), m_offset(offset), m_write(write), m_on_success(on_success), m_on_failure(on_failure) {}

    u8* buffer() const { return m_buffer; }

    u64 size() const { return m_size; }

    u64 offset() const { return m_offset; }

    bool write() const { return m_write; }

    Callable<void, RefPtr<BlockDeviceAsyncRequest>> on_success() const { return m_on_success; }

    Callable<void, RefPtr<BlockDeviceAsyncRequest>> on_failure() const { return m_on_failure; }
  };

  class BlockDevice : public Device {
    ArrayList<RefPtr<BlockDeviceAsyncRequest>> m_requests;
    //FIXME: This should be atomic.
    s64 m_next_request_id = 0;

  public:
    explicit BlockDevice(RefPtr<UnderlyingDevice> underlying_device, ArrayList<u64>&& interrupts) : Device(move(underlying_device), move(interrupts), DeviceType::Block, true) {}

    ErrorOr<void> write_async(u8* buffer, u64 size, u64 offset, Callable<void, RefPtr<BlockDeviceAsyncRequest>> on_success, Callable<void, RefPtr<BlockDeviceAsyncRequest>> on_failure);
    ErrorOr<void> read_async(u8* buffer, u64 size, u64 offset, Callable<void, RefPtr<BlockDeviceAsyncRequest>> on_success, Callable<void, RefPtr<BlockDeviceAsyncRequest>> on_failure);
    ErrorOr<void> write_poll(u8* buffer, u64 size, u64 offset);
    ErrorOr<void> read_poll(u8* buffer, u64 size, u64 offset);
    ErrorOr<void> init() override;

    ErrorOr<void> handle_interrupt(u64 interrupt_id) override;

  private:
  };

}// namespace Kernel
