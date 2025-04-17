/**
 * @file Memory.cpp
 * @author Matěj Bucek
 */
#include <Utils/Memory.h>

namespace Utils {
  extern "C" {
  void* memcpy(char* destination, const char* source, int n) {
    for(; n; n--) {
      *destination++ = *source++;
    }

    return destination;
  }

  void* memset(char* destination, int c, int n) {
    for(; n; n--) {
      *destination++ = c;
    }

    return destination;
  }

  int strcmp(const char* str1, const char* str2) {
    for(; *str1 && *str2; ++str1, ++str2) {
      if(*str1 != *str2) {
        return *str1 - *str2;
      }
    }

    return *str1 - *str2;
  }
  }
};// namespace Utils