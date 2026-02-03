#pragma once

#include <Kernel/Memory/PageTable.h>
#include <Kernel/Memory/VirtualMemory/MemoryRegion.h>
#include <Utils/Arrays/ArrayList.h>

namespace Kernel {
  using Utils::ArrayList;

  class AddressSpace {
    PageTable* m_page_table;
    ArrayList<RefPtr<MemoryRegion>> m_memory_regions;

  public:
    explicit AddressSpace(PageTable* page_table);
    ~AddressSpace();
  };

}// namespace Kernel
