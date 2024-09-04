//
// Created by matejbucek on 3.9.24.
//

#include <Kernel/Devices/Storage/BlockDevice.h>
#include <Kernel/Drivers/Storage/VirtIO/BlockIODriver.h>

namespace Kernel {
  ErrorOr<void> BlockDevice::init() {
    return m_underlying_device->as<VirtIODevice>()->init(0, 1, [](VirtQueue*, u64) {
      DebugConsole::println("BlockDevice: Initialising VirtQueue.");
    });
  }

  ErrorOr<void> BlockDevice::write_async(u8* buffer, u64 size, u64 offset, Callable<void, RefPtr<BlockDeviceAsyncRequest>> on_success, Callable<void, RefPtr<BlockDeviceAsyncRequest>> on_failure) {
    if(!has_driver()) {
      return ErrorOr<void>::create_error(Error::create_from_string("BlockDevice: No driver loaded."));
    }

    //FIXME: This should have wrapping around the request_id.
    auto request_id = m_next_request_id++;
    auto request = RefPtr<BlockDeviceAsyncRequest>(new BlockDeviceAsyncRequest(request_id, buffer, size, offset, false, move(on_success), move(on_failure)));
    m_requests.add(request);

    auto driver = m_driver->as<BlockIODriver>();
    return driver->write_async(request_id, buffer, size, offset);
  }

  ErrorOr<void> BlockDevice::read_async(u8* buffer, u64 size, u64 offset, Callable<void, RefPtr<BlockDeviceAsyncRequest>> on_success, Callable<void, RefPtr<BlockDeviceAsyncRequest>> on_failure) {
    if(!has_driver()) {
      return ErrorOr<void>::create_error(Error::create_from_string("BlockDevice: No driver loaded."));
    }

    auto request_id = m_next_request_id++;
    auto request = RefPtr<BlockDeviceAsyncRequest>(new BlockDeviceAsyncRequest(request_id, buffer, size, offset, false, move(on_success), move(on_failure)));
    m_requests.add(request);

    auto driver = m_driver->as<BlockIODriver>();
    return driver->read_async(request_id, buffer, size, offset);
  }

  ErrorOr<void> BlockDevice::write_poll(u8* buffer, u64 size, u64 offset) {
    if(!has_driver()) {
      return ErrorOr<void>::create_error(Error::create_from_string("BlockDevice: No driver loaded."));
    }

    auto driver = m_driver->as<BlockIODriver>();
    return driver->write_poll(buffer, size, offset);
  }

  ErrorOr<void> BlockDevice::read_poll(u8* buffer, u64 size, u64 offset) {
    if(!has_driver()) {
      return ErrorOr<void>::create_error(Error::create_from_string("BlockDevice: No driver loaded."));
    }

    auto driver = m_driver->as<BlockIODriver>();
    return driver->read_poll(buffer, size, offset);
  }

  ErrorOr<void> BlockDevice::handle_interrupt(u64) {
    if(!has_driver()) {
      return ErrorOr<void>::create_error(Error::create_from_string("BlockDevice: No driver loaded."));
    }

    auto driver = m_driver->as<BlockIODriver>();
    auto results_or_error = driver->notify();
    if(results_or_error.has_error()) {
      return ErrorOr<void>::create_error(results_or_error.get_error());
    }

    auto results = results_or_error.get_value();
    //TODO: Here we should notify places where the requests were initiated.
    for(auto& result: results) {
      auto request_or_error = m_requests.find_first_match([result](const RefPtr<BlockDeviceAsyncRequest>& request) -> bool {
        return request->id() == result->request_id();
      });

      if(request_or_error.has_error()) {
        DebugConsole::println("BlockDevice: Request not found.");
        continue;
      }

      auto request = request_or_error.get_value();
      if(result->type() == BlockDeviceAsyncRequestType::Ok) {
        request->on_success()(request);
      } else {
        request->on_failure()(request);
      }
    }

    return ErrorOr<void>::create({});
  }

}// namespace Kernel