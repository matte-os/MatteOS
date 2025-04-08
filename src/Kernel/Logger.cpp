#include <Kernel/Devices/Console/ConsoleDevice.h>
#include <Kernel/Devices/Device.h>
#include <Kernel/Devices/DeviceManager.h>
#include <Kernel/Logger.h>
#include <Kernel/SBI/SBI.h>
#include <Utils/Assertions.h>

namespace Kernel {
  static Logger* s_logger = nullptr;

  Logger::Logger() : m_output(LogOutput::SBI), m_level(LogLevel::Debug), m_console(nullptr), m_lines() {}

  void Logger::switch_to_sbi() {
    m_output = LogOutput::SBI;
  }

  void Logger::switch_to_logfile() {
    m_output = LogOutput::LogFile;
  }

  void Logger::switch_to_console() {
    m_output = LogOutput::Console;
  }

  void Logger::init() {
    if(s_logger == nullptr) {
      s_logger = new Logger();
    }
  }

  Logger& Logger::the() {
    runtime_assert(s_logger, "Logger: Not initialised.");
    return *s_logger;
  }

  void Logger::log(const String& message, LogLevel level) {
    if(Utils::as_underlying(level) > Utils::as_underlying(m_level)) return;

    switch(m_output) {
      case LogOutput::SBI: {
        for(size_t i = 0; i < message.length(); i++) {
          SBI::sbi_console_putchar(message[i]);
        }
      } break;
      case LogOutput::Console: {
        if(m_console) {
          m_console->write(message);
        }
      } break;
      case LogOutput::LogFile: {
        m_lines.add(message);
      } break;
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
    m_output = LogOutput::Console;
    log("Logger: Console device found. Switching!\n");
  }

  void Logger::log(char c) {
    if(m_console) {
      m_console->write(c);
    } else {
      SBI::sbi_console_putchar(c);
    }
  }

  void Logger::print_logfile() {
    auto previous = m_output;
    m_output = LogOutput::Console;
    for(const auto& message: m_lines) {
      log(message, m_level);
    }
    m_output = previous;
  }

  void Logger::log_to_console(const String& message) {
    auto previous = m_output;
    m_output = LogOutput::Console;
    log(message, m_level);
    m_output = previous;
  }
}// namespace Kernel
