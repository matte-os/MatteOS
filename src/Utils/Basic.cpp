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
    result = result * base + str[i] - '0';
  }

  return sign * result;
}