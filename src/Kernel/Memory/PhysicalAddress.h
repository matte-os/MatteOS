/**
 * @file PhysicalAddress.h
 * @author Matěj Bucek
 *
 * This file is based on the RISC-V specification.
 */
#pragma once

#include <Utils/Types.h>

namespace Kernel {
  struct PhysicalAddress {
    u64 offset   : 12;
    u64 ppn0     : 9;
    u64 ppn1     : 9;
    u64 ppn2     : 26;
    u64 reserved : 8;

    PhysicalAddress(u64 value) { *(u64*) this = value; };
    [[nodiscard]] u64 get_value() const { return *(u64*) this; }
  };

  static_assert(sizeof(PhysicalAddress) == 8, "PhysicalAddress size is not 8 bytes");
};// namespace Kernel