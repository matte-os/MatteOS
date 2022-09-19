//
// Created by matejbucek on 2.9.22.
//

#pragma once
#include <Utils/Types.hh>

namespace Kernel::Memory {
    enum class MemoryRegionType {
        CONVENTIONAL,
        VIRTIO
    };
    class MemoryRegion {
    private:
        MemoryRegionType type;
        u64 address;
        u64 size;
    public:
        MemoryRegion(){};
        MemoryRegion(MemoryRegionType, u64, u64, bool);
        ~MemoryRegion();
        MemoryRegionType getType();
        u64 getAddress();
        u64 getSize();
    };
}

