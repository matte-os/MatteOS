/**
 * @file Memory.h
 * @author Matěj Bucek (matejbucek)
 */
#pragma once

#include <Utils/Types.h>

namespace Utils {
  extern "C" {
  void* memcpy(char* destination, const char* source, int n);
  void* memset(char* destination, int c, int n);
  int strcmp(const char* str1, const char* str2);
  bool memcmp(const void* ptr1, const void* ptr2, size_t num);
  }

  template<typename T>
  T&& move(T& arg) {
    return static_cast<T&&>(arg);
  }

};// namespace Utils