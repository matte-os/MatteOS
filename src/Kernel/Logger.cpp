#include <Kernel/Devices/Console/ConsoleDevice.h>
#include <Kernel/Devices/Device.h>
#include <Kernel/Devices/DeviceManager.h>
#include <Kernel/Logger.h>
#include <Kernel/SBI/SBI.h>
#include <Utils/Assertions.h>

namespace Kernel {
  static Logger* s_logger = nullptr;

  void Logger::init() {
    if(s_logger == nullptr) {
      s_logger = new Logger();
    }
  }

  Logger& Logger::the() {
    runtime_assert(s_logger, "Logger: Not initialised.");
    return *s_logger;
  }

  void Logger::log(const String& message) {
    if(m_console) {
      m_console->write(message);
    } else {
      for(size_t i = 0; i < message.length(); i++) {
        SBI::sbi_console_putchar(message[i]);
      }
    }
  }

  void Logger::try_console_lookup() {
    if(m_console) {
      return;
    }

    auto devices = DeviceManager::the().get_devices_of_type(DeviceType::Console);
    if(devices.size() == 0) {
      return;
    }

    m_console = devices[0]->as<ConsoleDevice>();
    log("Logger: Console device found. Switching!\n");
  }

  void Logger::log(char c) {
    if(m_console) {
      m_console->write(c);
    } else {
      SBI::sbi_console_putchar(c);
    }
  }
}// namespace Kernel
