#pragma once

#include <Utils/Types.h>

namespace Kernel {
  struct VirtualAddress {
    u64 offset   : 12;
    u64 vpn0     : 9;
    u64 vpn1     : 9;
    u64 vpn2     : 9;
    u64 reserved : 25;

    VirtualAddress(u64 offset, u64 vpn0, u64 vpn1, u64 vpn2, u64 reserved) {
      this->offset = offset;
      this->vpn0 = vpn0;
      this->vpn1 = vpn1;
      this->vpn2 = vpn2;
      this->reserved = reserved;
    };

    VirtualAddress(u64 value) {
      *(u64*) this = value;
    };
  };

  static_assert(sizeof(VirtualAddress) == 8, "VirtualAddress size is not 8 bytes");
};// namespace Kernel