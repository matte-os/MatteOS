#pragma once

#include <Utils/Forwards.h>
#include <Utils/Types.h>

namespace Utils {
  class DebugConsole {
  public:
    static void print(char);
    static void print(const char*);
    static void print(String);
    static void println(const char*);
    static void println(String);
    static void print_number(u64 number, u32 base);
    static void print_ln_number(u64 number, u32 base);
    static void switch_to_device();
  };
};// namespace Utils