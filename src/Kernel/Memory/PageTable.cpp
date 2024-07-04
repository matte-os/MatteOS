#include <Kernel/Memory/PageTable.h>
#include <Kernel/Memory/PageTableEntry.h>
#include <Utils/Basic.h>

namespace Kernel::Memory {
  void PageTable::debug_output() {
    char buffer[64];
    for(int i = 0; i < 520; i++) {
      PageTableEntry& entry = this->m_entries[i];
      u64 value = entry.get_value();
      if(value != 0) {
        itoa(buffer, i, 10);
        DebugConsole::print("The value of ");
        DebugConsole::print(buffer);
        DebugConsole::print(". PTE is: ");
        itoa(buffer, value, 2);
        DebugConsole::println(buffer);
      }
    }
  }
};// namespace Kernel::Memory