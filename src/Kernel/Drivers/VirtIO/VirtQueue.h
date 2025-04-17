/**
 * @file VirtQueue.h
 * @author Matěj Bucek
 *
 * This file is based on the VirtIO specification
 */
#pragma once

#include <Kernel/Memory/MemoryManager.h>
#include <Utils/Arrays/Array.h>
#include <Utils/Endian.h>
#include <Utils/Pointers/RefCounted.h>
#include <Utils/Types.h>

#define VIRTIO_RING_SIZE 128
#define PAGE_SIZE 4096

namespace Kernel {
  using Utils::Array;
  using Utils::Endian;
  using Utils::Endianness;

  enum class VirtQueueDescriptorFlags {
    Next = 1,
    Write = 2,
    Indirect = 4,
  };

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

  class VirtQueueArray {
  private:
    VirtQueue** m_queues;
    u32 m_number_of_queues;

  public:
    VirtQueueArray() = default;

    VirtQueueArray(u32 num_queues) : m_queues(new VirtQueue*[num_queues]), m_number_of_queues(num_queues) {}

    VirtQueueArray(const VirtQueueArray& other) = delete;
    VirtQueueArray(VirtQueueArray&& other) noexcept = delete;

    VirtQueueArray& operator=(const VirtQueueArray& other) = delete;

    VirtQueueArray& operator=(VirtQueueArray&& other) noexcept {
      if(this != &other) {
        m_queues = other.m_queues;
        m_number_of_queues = other.m_number_of_queues;
        other.m_queues = nullptr;
        other.m_number_of_queues = 0;
      }
      return *this;
    }

    [[nodiscard]] VirtQueue** get_queues() const {
      return m_queues;
    }

    [[nodiscard]] u32 get_number_of_queues() const {
      return m_number_of_queues;
    }

    VirtQueue*& operator[](u32 index) {
      runtime_assert(index < m_number_of_queues, "VirtQueueArray: Index out of bounds.");
      return m_queues[index];
    }

    ~VirtQueueArray() {
      for(u32 i = 0; i < m_number_of_queues; i++) {
        MemoryManager::the().dealloc(reinterpret_cast<uintptr_t*>(m_queues[i]));
      }
      delete m_queues;
    }
  };


}// namespace Kernel
