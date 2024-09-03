//
// Created by matejbucek on 3.9.24.
//

#include <Kernel/Devices/BlockDevice.h>
#include <Kernel/Drivers/VirtIO/BlockIODriver.h>

namespace Kernel {
  ErrorOr<void> BlockDevice::init() {
    return m_underlying_device->as<VirtIODevice>()->init(0, 1, [](VirtQueue*, u64) {
      DebugConsole::println("BlockDevice: Initialising VirtQueue.");
    });
  }

  ErrorOr<void> BlockDevice::write(u8* buffer, u64 size, u64 offset) {
    if(!has_driver()) {
      return ErrorOr<void>::create_error(Error::create_from_string("BlockDevice: No driver loaded."));
    }

    auto driver = m_driver->as<BlockIODriver>();
    return driver->write(buffer, size, offset);
  }

  ErrorOr<void> BlockDevice::read(u8* buffer, u64 size, u64 offset) {
    if(!has_driver()) {
      return ErrorOr<void>::create_error(Error::create_from_string("BlockDevice: No driver loaded."));
    }

    auto driver = m_driver->as<BlockIODriver>();
    return driver->read(buffer, size, offset);
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

  ErrorOr<void> BlockDevice::handle_interrupt(u64 interrupt_id) {
    return Device::handle_interrupt(interrupt_id);
  }
}// namespace Kernel