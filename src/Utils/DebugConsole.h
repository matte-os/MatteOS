#pragma once

#include <Utils/Strings/String.h>
#include <Utils/Types.h>

namespace Utils {
  using namespace Strings;
  class DebugConsole {
  public:
    //static void dprintf(const char *, ...);
    static void print(char);
    static void print(const char*);
    static void println(const char*);
    template<typename T, typename... Args>
    static void printf(const char* format, T arg, Args&&... args) {
      for(size_t i = 0; i < String::calculate_size(format); i++) {
        if(format[i] == '{') {
          print_number(arg, 10);
          printf(format + i + 2, args...);
          return;
        }
        print(format[i]);
      }
    }
    template<typename... Args>
    static void printf(const char* format, Args&&... args) {
      printf(format, args...);
    }
    static void printf(const char* format) {
      print(format);
    }
    static void dbgln(const char*, ...);
    static void print_number(u64 number, u32 base);
    static void print_ln_number(u64 number, u32 base);
  };
};// namespace Utils