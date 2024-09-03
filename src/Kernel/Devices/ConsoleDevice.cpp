#include <Kernel/Devices/ConsoleDevice.h>

namespace Kernel {
  ErrorOr<void> ConsoleDevice::init() {
    return Device::init();
  }

  ErrorOr<void> ConsoleDevice::handle_interrupt(u64 interrupt_id) {
    return Device::handle_interrupt(interrupt_id);
  }

  ErrorOr<void> ConsoleDevice::write(String message) {
    DebugConsole::println(message.to_cstring());
    return ErrorOr<void>::create({});
  }

  ErrorOr<String> ConsoleDevice::read() {
    return ErrorOr<String>::create_error(Error::create_from_string("ConsoleDevice: Not implemented."));
  }

}// namespace Kernel