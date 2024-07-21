#include <Utils/Hashing/HashFunctions.h>

namespace Utils {

  constexpr u32 djb2(const char* str) {
    u32 hash = 5381;
    int c;

    while ((c = *str++)) {
      hash = ((hash << 5) + hash) + c;
    }

    return hash;
  }

}// namespace Utils