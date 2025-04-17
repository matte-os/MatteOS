/**
 * @file Endian.h
 * @author Matěj Bucek (matejbucek)
 */
#pragma once

#include <Utils/Types.h>
namespace Utils {
  enum class Endianness {
    Big,
    Little
  };
  template<typename T, Endianness E>
  class Endian {
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
    Endian(T value) {
      if(E == Endianness::Big && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) {
        this->value = swap_endian(value);
      } else if(E == Endianness::Little && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__) {
        this->value = swap_endian(value);
      } else {
        this->value = value;
      }
    }

    T operator*() {
      return in_system_endian();
    }

    Endian& operator=(T val) {
      if(E == Endianness::Big && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) {
        value = swap_endian(val);
      } else if(E == Endianness::Little && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__) {
        value = swap_endian(val);
      } else {
        value = val;
      }
      return *this;
    }

    void emplace(T value) {
      this->value = value;
    }

    operator T() {
      return **this;
    }

    operator T() const {
      return **this;
    }

    void operator++() {
      auto value = in_system_endian() + 1;
      this->value = to_specific_endian(value);
    }

    void operator++(int) {
      auto value = in_system_endian() + 1;
      this->value = to_specific_endian(value);
    }

    void operator--(int) {
      auto value = in_system_endian() - 1;
      this->value = to_specific_endian(value);
    }

    void operator--() {
      auto value = in_system_endian() - 1;
      this->value = to_specific_endian(value);
    }

    T operator+(T val) {
      return in_system_endian() + val;
    }

    T operator-(T val) {
      return in_system_endian() - val;
    }

    T operator%(T val) {
      return in_system_endian() % val;
    }

    T operator/(T val) {
      return in_system_endian() / val;
    }

  private:
    T in_system_endian() {
      if(E == Endianness::Big && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) {
        return swap_endian(value);
      } else if(E == Endianness::Little && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__) {
        return swap_endian(value);
      } else {
        return value;
      }
    }

    T in_specified_endian() {
      return value;
    }

    T to_specific_endian(T value) {
      if(E == Endianness::Big && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) {
        return swap_endian(value);
      } else if(E == Endianness::Little && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__) {
        return swap_endian(value);
      } else {
        return value;
      }
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