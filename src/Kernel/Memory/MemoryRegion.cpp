/**
 * @file MemoryRegion.cpp
 * @author Matěj Bucek
 */
#include <Kernel/Memory/MemoryRegion.h>

namespace Kernel {
  MemoryRegion::MemoryRegion(MemoryRegionType type, u64 address, u64 size, bool addr = false) {
    this->m_type = type;
    this->m_start_address = address;
    if(!addr) {
      this->m_size = size;
    } else {
      this->m_size = size - address;
    }
  }
}// namespace Kernel