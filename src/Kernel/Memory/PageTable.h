#pragma once

#include <Kernel/Memory/VirtualAddress.h>
#include <Kernel/Memory/PageTableEntry.h>
#include <Kernel/Memory/PhysicalAddress.h>
#include <Utils/DebugConsole.h>
#include <Utils/Errors/ErrorOr.h>

using Utils::DebugConsole;
using Utils::ErrorOr;

namespace Kernel {
  struct PageTable {
    PageTableEntry m_entries[512];
    PageTable() = default;

    ErrorOr<PhysicalAddress> translate(VirtualAddress address);
    void debug_output();
  };
};// namespace Kernel