//
// Created by matejbucek on 8.7.24.
//

#pragma once

#include <Utils/Endian.h>
#include <Utils/Types.h>

namespace Kernel {
  using Utils::Endian;
  using Utils::Endianness;

  struct VirtQueueDescriptor {
    Endian<u64, Endianness::Little> address;
    Endian<u32, Endianness::Little> length;
    Endian<u16, Endianness::Little> flags;
    Endian<u16, Endianness::Little> next;
  };

  struct IndirectDescriptorTable {
  };

  struct VirtQueueAvailable {
    Endian<u16, Endianness::Little> flags;
    Endian<u16, Endianness::Little> index;
    Endian<u16, Endianness::Little> ring[];
  };

  struct VirtQueueUsed {
    Endian<u16, Endianness::Little> flags;
    Endian<u16, Endianness::Little> index;
    struct VirtQueueUsedElement {
      Endian<u32, Endianness::Little> id;
      Endian<u32, Endianness::Little> length;
    } ring[];
  };

}// namespace Kernel
