/**
 * @file Logger.h
 * @author Matěj Bucek
 */
#pragma once
#include <Kernel/Forwards.h>
#include <Utils/Arrays/ArrayList.h>
#include <Utils/Arrays/CircularStringBuffer.h>
#include <Utils/Strings/Formatter.h>
#include <Utils/Strings/String.h>

namespace Kernel {
  using Utils::CircularStringBuffer;
  using Utils::format;
  using Utils::String;
  using Utils::StringView;
#ifdef __GNUC__
#define GET_CLASS_NAME (__PRETTY_FUNCTION__)
#else
#define GET_CLASS_NAME (__FUNCTION__)
#endif

  enum class LogLevel : u8 {
    Error,
    Warning,
    Info,
    Debug
  };

  class Logger {
    // ReSharper disable once CppDefaultedSpecialMemberFunctionIsImplicitlyDeleted
    Logger();
    ~Logger() = default;

    static constexpr size_t BufferSize = 10000;

    enum class LogOutput {
      SBI,
      Console,
      LogFile,
    } m_output = LogOutput::SBI;

    LogLevel m_level = LogLevel::Debug;

    RefPtr<ConsoleDevice> m_console;
    CircularStringBuffer m_buffer;

  public:
    static void init();
    static Logger& the();

    void try_console_lookup();
    void switch_to_sbi();
    void switch_to_logfile();
    void switch_to_console();

    void log(const String& message, LogLevel level = LogLevel::Debug, bool include_timestamp = true);
    void log(char c);

    void log_to_console(const String& message);

    void print_logfile();

    size_t get_buffer_size();
  };

  template<typename... Args>
  void log(const char* fmt, const Args&... args) {
    Logger::the().log(format(fmt, args...));
  }

  template<typename... Args>
  void logln(const char* fmt, const Args&... args) {
    log(fmt, args...);
    Logger::the().log("\n", LogLevel::Debug, false);
  }

  template<typename... Args>
  void logln_direct(const char* fmt, const Args&... args) {
    Logger::the().log_to_console(format(fmt, args...));
  }

#define dbglog(...) Kernel::log(__VA_ARGS__)

#define dbgln(...) Kernel::logln(__VA_ARGS__)

#define dbglog_direct(...) Kernel::logln_direct(__VA_ARGS__)
}// namespace Kernel
