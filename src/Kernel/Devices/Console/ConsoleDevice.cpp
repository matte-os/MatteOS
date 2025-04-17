/**
* @file ConsoleDevice.cpp
 * @author Matěj Bucek
 */
#include <Kernel/Devices/Console/ConsoleDevice.h>
#include <Kernel/Drivers/Console/ConsoleDeviceDriver.h>
#include <Kernel/Process/ProcessManager.h>
#include <Kernel/Process/Request.h>
#include <Utils/Errors/ErrorOr.h>

namespace Kernel {
  ErrorOr<void> ConsoleDevice::init() {
    create_dev_inode();
    return Device::init();
  }

  ErrorOr<void> ConsoleDevice::handle_interrupt(u64 interrupt_id) {
    if(!m_driver) {
      return ErrorOr<void>::create_error(Error::create_from_string("ConsoleDevice: No driver."));
    }

    m_driver->as<ConsoleDeviceDriver>()->handle_interrupt(interrupt_id);

    for(auto request : m_requests) {
      if(m_driver->as<ConsoleDeviceDriver>()->line_available()) {
        ProcessManager::the().unblock(request->get_pid(), request->get_tid());
      }
    }

    return {};
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

    return m_driver->as<ConsoleDeviceDriver>()->read_line();
  }

  ErrorOr<void> ConsoleDevice::write(char c) {
    if(!m_driver) {
      return ErrorOr<void>::create_error(Error::create_from_string("ConsoleDevice: No driver."));
    }

    m_driver->as<ConsoleDeviceDriver>()->write(c);
    return ErrorOr<void>::create({});
  }

  ErrorOr<Request<String>> ConsoleDevice::read_async() {
    auto error_or_read = read();
    if(error_or_read.has_error()) {
      auto request = Request<String>::create_blocked();
      m_requests.add(request.get_context());
      return request;
    }

    return Request<String>::create_finalized(error_or_read.get_value());
  }

}// namespace Kernel