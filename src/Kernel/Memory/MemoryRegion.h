//
// Created by matejbucek on 2.9.22.
//

#pragma once
#include <Utils/Types.h>

namespace Kernel::Memory {
    enum class MemoryRegionType {
        CONVENTIONAL,
        VIRTIO
    };
    class MemoryRegion {
    private:
        MemoryRegionType m_type;
        u64 m_address;
        u64 m_size;
    public:
        MemoryRegion(){};
        MemoryRegion(MemoryRegionType, u64, u64, bool);
        ~MemoryRegion();
        MemoryRegionType get_type();
        u64 get_address();
        u64 get_size();
    };
}

