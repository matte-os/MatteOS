//
// Created by matejbucek on 8.7.24.
//

#pragma once

#include <Utils/Endian.h>
#include <Utils/Types.h>

#define VIRTIO_RING_SIZE 128
#define PAGE_SIZE 4096

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
    Endian<u16, Endianness::Little> ring[VIRTIO_RING_SIZE];
    Endian<u16, Endianness::Little> event;
  };

  struct VirtQueueUsed {
    Endian<u16, Endianness::Little> flags;
    Endian<u16, Endianness::Little> index;
    struct VirtQueueUsedElement {
      Endian<u32, Endianness::Little> id;
      Endian<u32, Endianness::Little> length;
    } ring[VIRTIO_RING_SIZE];
    Endian<u16, Endianness::Little> event;
  };

  struct VirtQueue {
    VirtQueueDescriptor descriptors[VIRTIO_RING_SIZE];
    VirtQueueAvailable available;
    u8 padding[PAGE_SIZE - sizeof(VirtQueueDescriptor) * VIRTIO_RING_SIZE - sizeof(VirtQueueAvailable)];
    VirtQueueUsed used;
  };

}// namespace Kernel
