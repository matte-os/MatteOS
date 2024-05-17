#pragma once

namespace Utils {
  extern "C" {
  void* memcpy(char* dst, const char* src, int n);
  void* memset(char* dst, int ch, int n);
  void* memmov(char* dst, char* src, int base, int until, int to);
  }

  template<typename T>
  T&& move(T& arg) {
    return static_cast<T&&>(arg);
  }

};// namespace Utils