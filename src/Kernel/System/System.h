//
// Created by matejbucek on 1.9.22.
//

#pragma once
#include <Utils/Types.hh>
#include <Kernel/Firmware/FDT/fdt.h>
#include <Utils/Arrays/ArrayList.h>
#include <Kernel/Memory/MemoryRegion.h>

using Utils::ArrayList;
using Kernel::Memory::MemoryRegion;

namespace Kernel::System {
    class System {
    private:
        size_t numberOfHarts;
        ArrayList<MemoryRegion> memoryRegions;
    public:
        static void init();
        static System& the();
        void parseFDT(fdt_header*);
        size_t getNumberOfHarts();
    private:
        System();
        ~System() = default;
        void initHarts();
    };

} // Kernel