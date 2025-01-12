#include <Kernel/Devices/Console/ConsoleDevice.h>
#include <Kernel/Drivers/Console/ConsoleDeviceDriver.h>
#include <Utils/Errors/ErrorOr.h>

namespace Kernel {
  ErrorOr<void> ConsoleDevice::init() {

    return Device::init();
  }

  ErrorOr<void> ConsoleDevice::handle_interrupt(u64 interrupt_id) {
    if(!m_driver) {
      return ErrorOr<void>::create_error(Error::create_from_string("ConsoleDevice: No driver."));
    }

    m_driver->as<ConsoleDeviceDriver>()->handle_interrupt(interrupt_id);
    return ErrorOr<void>::create({});
  }

  ErrorOr<void> ConsoleDevice::write(String message) {
    if(!m_driver) {
      return ErrorOr<void>::create_error(Error::create_from_string("ConsoleDevice: No driver."));
    }

    m_driver->as<ConsoleDeviceDriver>()->write(message);
    return ErrorOr<void>::create({});
  }

  ErrorOr<String> ConsoleDevice::read() {
    if(!m_driver) {
      return ErrorOr<String>::create_error(Error::create_from_string("ConsoleDevice: No driver."));
    }

    return m_driver->as<ConsoleDeviceDriver>()->read();
  }

  ErrorOr<char> ConsoleDevice::read_char() {
    if(!m_driver) {
      return ErrorOr<char>::create_error(Error::create_from_string("ConsoleDevice: No driver."));
    }

    return m_driver->as<ConsoleDeviceDriver>()->read_char();
  }

  bool ConsoleDevice::supports_read_string() const {
    return m_driver && m_driver->as<ConsoleDeviceDriver>()->supports_read_string();
  }

  ErrorOr<void> ConsoleDevice::write(char c) {
    if(!m_driver) {
      return ErrorOr<void>::create_error(Error::create_from_string("ConsoleDevice: No driver."));
    }

    m_driver->as<ConsoleDeviceDriver>()->write(c);
    return ErrorOr<void>::create({});
  }
}// namespace Kernel