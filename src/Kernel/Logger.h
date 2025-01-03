#pragma once
#include <Kernel/Forwards.h>
#include <Utils/Strings/Formatter.h>
#include <Utils/Strings/String.h>

namespace Kernel {
  using Utils::format;
  using Utils::String;
  using Utils::StringView;
#ifdef __GNUC__
#define GET_CLASS_NAME (__PRETTY_FUNCTION__)
#else
#define GET_CLASS_NAME (__FUNCTION__)
#endif

  class Logger {
  private:
    Logger() = default;
    ~Logger() = default;

    RefPtr<ConsoleDevice> m_console;

  public:
    static void init();
    static Logger& the();

    void try_console_lookup();

    void log(const String& message);
    void log(char c);
  };

  template<typename... Args>
  void log(StringView prefix, const char* fmt, const Args&... args) {
    String message;
    for(size_t i = 0; i < prefix.length(); i++) {
      message += prefix[i];
      if(prefix[i] == ':') {
        message += ' ';
        break;
      }
    }
    message += format(fmt, args...);

    Logger::the().log(message);
  }

  template<typename... Args>
  void logln(StringView prefix, const char* fmt, const Args&... args) {
    log(prefix, fmt, args...);
    Logger::the().log("\n");
  }

#define dbglog(...) Kernel::log(GET_CLASS_NAME, __VA_ARGS__)

#define dbgln(...) Kernel::logln(GET_CLASS_NAME, __VA_ARGS__)
}// namespace Kernel
