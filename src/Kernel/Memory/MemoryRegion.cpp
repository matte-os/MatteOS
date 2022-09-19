//
// Created by matejbucek on 2.9.22.
//

#include <Kernel/Memory/MemoryRegion.h>

namespace Kernel::Memory {
    MemoryRegion::MemoryRegion(MemoryRegionType type, u64 address, u64 size, bool addr = false) {
        this->type = type;
        this->address = address;
        if(!addr) {
            this->size = size;
        } else {
            this->size = size - address;
        }
    }

    MemoryRegion::~MemoryRegion() {}
    u64 MemoryRegion::getSize() { return size; }
    u64 MemoryRegion::getAddress() { return address; }
    MemoryRegionType MemoryRegion::getType() { return type; }
}