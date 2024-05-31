//
// Created by matejbucek on 2.9.22.
//

#include <Kernel/Memory/MemoryRegion.h>

namespace Kernel::Memory {
  MemoryRegion::MemoryRegion(MemoryRegionType type, u64 address, u64 size, bool addr = false) {
    this->m_type = type;
    this->m_start_address = address;
    if(!addr) {
      this->m_size = size;
    } else {
      this->m_size = size - address;
    }
  }
}// namespace Kernel::Memory