/**
 * @file Basic.cpp
 * @author Matěj Bucek (matejbucek)
 *
 * This file was inspired by Samy Pessé, How to make a Computer Operating System,
 * https://github.com/SamyPesse/How-to-Make-a-Computer-Operating-System/blob/master/src/kernel/runtime/itoa.cc
 */
#include <Utils/Basic.h>

void itoa(char* buf, unsigned long int n, int base) {
  unsigned long int tmp;
  int i, j;

  tmp = n;
  i = 0;

  do {
    tmp = n % base;
    buf[i++] = (tmp < 10) ? (tmp + '0') : (tmp + 'a' - 10);
  } while(n /= base);
  buf[i--] = 0;

  for(j = 0; j < i; j++, i--) {
    tmp = buf[j];
    buf[j] = buf[i];
    buf[i] = tmp;
  }
}

size_t atoi(const char* str, size_t base) {
  size_t result = 0;
  size_t i = 0;
  size_t sign = 1;

  if(str[0] == '-') {
    sign = -1;
    i++;
  }

  for(; str[i] != 0; i++) {
    if(str[i] >= '0' && str[i] <= '9') {
      result = result * base + (str[i] - '0');
    } else if(str[i] >= 'a' && str[i] <= 'f') {
      result = result * base + (str[i] - 'a' + 10);
    } else if(str[i] >= 'A' && str[i] <= 'F') {
      result = result * base + (str[i] - 'A' + 10);
    } else {
      return sign * result;
    }
  }
  return sign * result;
}