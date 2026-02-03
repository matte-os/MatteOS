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

  bool memcmp(const void* ptr1, const void* ptr2, size_t num) {
    const u8* p1 = static_cast<const u8*>(ptr1);
    const u8* p2 = static_cast<const u8*>(ptr2);
    for(size_t i = 0; i < num; i++) {
      if(p1[i] != p2[i]) {
        return false;
      }
    }
    return true;
  }
  }
};// namespace Utils