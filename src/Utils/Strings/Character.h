#pragma once

#include <Utils/Types.h>

class Character {
public:
  static bool is_ascii(u32 code_point) {
    return code_point <= 0x7F;
  }

  static bool is_printable(u32 code_point) {
    return (code_point >= 0x20 && code_point <= 0x7E) || (code_point >= 0xA0 && code_point <= 0xD7FF) || (code_point >= 0xE000 && code_point <= 0xFFFD) || (code_point >= 0x10000 && code_point <= 0x10FFFF);
  }
};
