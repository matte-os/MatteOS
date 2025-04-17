#include <Kernel/SBI/SBI.h>
#include <Utils/Basic.h>
#include <Utils/DebugConsole.h>
#include <Utils/Strings/String.h>
#include <Kernel/Logger.h>

namespace Utils {
  static bool use_device = false;

  void DebugConsole::print(const char c) {
    if(use_device) {
      Kernel::Logger::the().log(c);
      return;
    }
    Kernel::SBI::sbi_console_putchar(c);
  }

  void DebugConsole::print(const char* str) {
    for(size_t i = 0; 1; i++) {
      char character = str[i];
      if(character == '\0') {
        return;
      }
      print(character);
    }
  }

  void DebugConsole::println(const char* str) {
    for(size_t i = 0; 1; i++) {
      char character = str[i];
      if(character == '\0') {
        print('\n');
        return;
      }
      print(character);
    }
  }

  void DebugConsole::print_number(u64 number, u32 base) {
    char buffer[64];
    itoa(buffer, number, base);
    print(buffer);
  }

  void DebugConsole::print_ln_number(u64 number, u32 base) {
    print_number(number, base);
    print('\n');
  }

  void DebugConsole::print(String value) {
    for(size_t i = 0; i < value.length(); i++) {
      print(value[i]);
    }
  }

  void DebugConsole::println(String value) {
    for(size_t i = 0; i < value.length(); i++) {
      print(value[i]);
    }
    print('\n');
  }

  void DebugConsole::switch_to_device() {
    use_device = true;
  }

  /*
  void DebugConsole::dbgln(const char* fmt, ...) {
            va_list args;
        va_start(args, fmt);

        va_end(args);
  } */
};// namespace Utils