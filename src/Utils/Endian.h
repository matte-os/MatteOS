#pragma once

#include <Utils/Types.h>
namespace Utils {
  enum class Endianness {
    Big,
    Little
  };
  template<typename T, Endianness E>
  class Endian {
  private:
    T value;
    T swap_endian(T val) {
      if(sizeof(T) == 8) {
        return __builtin_bswap64(val);
      } else if(sizeof(T) == 4) {
        return __builtin_bswap32(val);
      } else if(sizeof(T) == 2) {
        return __builtin_bswap16(val);
      }
      return val;
    }

  public:
    Endian() = default;
    explicit Endian(T value) : value(value) {}

    T operator*() {
      if(E == Endianness::Big && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) {
        return swap_endian(value);
      } else if(E == Endianness::Little && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__) {
        return swap_endian(value);
      } else {
        return value;
      }
    }

    Endian<T, E>& operator=(T val) {
      if(E == Endianness::Big && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) {
        value = swap_endian(val);
      } else if(E == Endianness::Little && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__) {
        value = swap_endian(val);
      } else {
        value = val;
      }
      return *this;
    }
  };

  template<typename T, typename OffsetType, Endianness E, uintptr_t From>
  class Offset {
  private:
    Endian<OffsetType, E> offset;
  public:
    explicit Offset(OffsetType offset) : offset(offset) {}

    T operator*() {
      return *reinterpret_cast<T*>(From + *offset);
    }
  };
}// namespace Utils