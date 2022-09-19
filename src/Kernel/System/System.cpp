//
// Created by matejbucek on 1.9.22.
//

#include <Kernel/System/System.h>
#include <Kernel/sbi/sbi.hh>
#include <Utils/Basic.hh>
#include <Utils/DebugConsole.hh>

namespace Kernel::System {
    static System* system;

    void System::init() {
        system = new System;
    }

    System& System::the() {
        return *system;
    }

    System::System() {
        memoryRegions = ArrayList<MemoryRegion>();
        numberOfHarts = 0;
        initHarts();
        //Utils::DebugConsole::println("System: Initialised.");
    }

    void System::parseFDT(fdt_header* header) {
        Utils::DebugConsole::println("System: plskyyy");
        auto magic = header->magic;
        magic = __builtin_bswap32(header->magic);
        Utils::DebugConsole::printLnNumber(magic, 16);
        if(__builtin_bswap32(header->magic) == 0xd00dfeed) {
            Utils::DebugConsole::println("Jsme tu");
            Utils::DebugConsole::print("FDT Version: ");
            Utils::DebugConsole::printLnNumber(__builtin_bswap32(header->version), 10);
            auto totalSize = __builtin_bswap32(header->totalsize);
            auto memoryOffset = __builtin_bswap32(header->off_mem_rsvmap);
            auto* memoryEntry = (fdt_reserve_entry*) (((u64) header) + memoryOffset);
            Utils::DebugConsole::printLnNumber(__builtin_bswap64(memoryEntry->address), 16);
            Utils::DebugConsole::printLnNumber(__builtin_bswap64(memoryEntry->size), 16);
            while(memoryEntry->address != 0 && memoryEntry->size != 0) {
                memoryRegions.add(MemoryRegion(Memory::MemoryRegionType::CONVENTIONAL, __builtin_bswap64(memoryEntry->address),
                                               __builtin_bswap64(memoryEntry->size), false));
                memoryEntry++;
                Utils::DebugConsole::println("Ahojkyy");
            }

            Utils::DebugConsole::print("System: The size of MemoryRegions is ");
            Utils::DebugConsole::printLnNumber(memoryRegions.getSize(), 10);
            Utils::DebugConsole::print("System: The first memory region address ");
            //Utils::DebugConsole::printNumber(memoryRegions[0].getAddress(), 16);
            Utils::DebugConsole::print(", size ");
            //Utils::DebugConsole::printLnNumber(memoryRegions[0].getSize(), 16);
        }
    }

    size_t System::getNumberOfHarts() {
        return numberOfHarts;
    }

    void System::initHarts() {
        //FIXME: We should get this information out of a fdt. This is a fast hack to get it in dirty way.
        for(int i = 0; i < 100; i++) {
            auto hart = SBI::sbi_hart_get_status(i);
            if (hart.error == -3) {
                Utils::DebugConsole::print("System: sbi_hart_get_status returned ");
                Utils::DebugConsole::printLnNumber(hart.error, 10);
                break;
            } else {
                numberOfHarts++;
            }
        }
        Utils::DebugConsole::print("System: The system has ");
        Utils::DebugConsole::printNumber(numberOfHarts, 10);
        Utils::DebugConsole::println(" harts.");
    }
} // Kernel