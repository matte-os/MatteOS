//
// Created by matejbucek on 2.9.22.
//

#include <Kernel/Memory/MemoryRegion.h>

namespace Kernel::Memory {
    MemoryRegion::MemoryRegion(MemoryRegionType type, u64 address, u64 size, bool addr = false) {
        this->m_type = type;
        this->m_address = address;
        if(!addr) {
            this->m_size = size;
        } else {
            this->m_size = size - address;
        }
    }

    MemoryRegion::~MemoryRegion() {}
    u64 MemoryRegion::get_size() { return m_size; }
    u64 MemoryRegion::get_address() { return m_address; }
    MemoryRegionType MemoryRegion::get_type() { return m_type; }
}