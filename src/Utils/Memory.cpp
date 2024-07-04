#include <Utils/Memory.h>
namespace Utils {
  extern "C" {

  void* memcpy(char* dst, const char* src, int n) {
    char* p = dst;
    while(n--)
      *dst++ = *src++;
    return p;
  }

  void* memset(char* dst, int ch, int n) {
    char* p = dst;
    while(n--)
      *dst++ = ch;
    return p;
  }

  void memmvo(char* dst, char* src, int base, int until, int to) {
    for(int i = base; i < until; i++) {
      dst[to + (i - base)] = src[i];
    }
  }

  int strcmp(const char* X, const char* Y) {
    while(*X) {
      // if characters differ, or end of the second string is reached
      if(*X != *Y) {
        break;
      }

      // move to the next pair of characters
      X++;
      Y++;
    }

    // return the ASCII difference after converting `char*` to `unsigned char*`
    return *(const unsigned char*) X - *(const unsigned char*) Y;
  }
  }
};// namespace Utils