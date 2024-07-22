//
// Created by matejbucek on 2.9.22.
//

#pragma once
#include <Utils/Types.h>

namespace Kernel {
  enum class MemoryRegionType : u8 {
    Uninitialized,
    Conventional,
    VirtIO
  };
  class MemoryRegion {
  private:
    MemoryRegionType m_type = MemoryRegionType::Uninitialized;
    u64 m_start_address{};
    u64 m_size{};

  public:
    MemoryRegion() = default;
    MemoryRegion(MemoryRegionType, u64, u64, bool);
    ~MemoryRegion() = default;
    [[nodiscard]] u64 get_size() const { return m_size; }
    [[nodiscard]] u64 get_start_address() const { return m_start_address; }
    [[nodiscard]] u64 get_end_address() const { return m_start_address + m_size; }
    [[nodiscard]] MemoryRegionType get_type() const { return m_type; }
  };
}// namespace Kernel
