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

  HashInput to_hash_input(const char* value) {
    return { value, String::calculate_size(value) };
  }

  HashInput to_hash_input(const String& value) {
    return { value.to_cstring(), value.length() };
  }

  HashInput to_hash_input(const StringView& value) {
    return { value.value(), value.length() };
  }

  HashInput to_hash_input(u64 value) {
    auto str = String::from_u64(value);
    return { str.to_cstring(), str.length() };
  }
}// namespace Utils