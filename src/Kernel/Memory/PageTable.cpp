/**
 * @file PageTable.cpp
 * @author Matěj Bucek
 */
#include <Kernel/Memory/PageTable.h>
#include <Kernel/Memory/PageTableEntry.h>
#include <Utils/Basic.h>
#include <Utils/Errors/ErrorOr.h>

using Utils::ErrorOr;
using Utils::Error;

namespace Kernel {
  void PageTable::debug_output() {
    char buffer[64];
    for(int i = 0; i < 512; i++) {
      PageTableEntry& entry = this->m_entries[i];
      u64 value = entry.get_value();
      if(entry.is_valid()) {
        itoa(buffer, i, 10);
        DebugConsole::print("The value of ");
        DebugConsole::print(buffer);
        DebugConsole::print(". PTE is: ");
        itoa(buffer, value, 2);
        DebugConsole::println(buffer);
      }
    }
  }
  ErrorOr<PhysicalAddress> PageTable::translate(const VirtualAddress address) {
    PageTableEntry* entry = &this->m_entries[address.vpn2];
    if(!entry->is_valid()) {
      return ErrorOr<PhysicalAddress>::create_error(Error::create_from_string("First entry is not valid"));
    }

    if(entry->is_leaf()) {
      return ErrorOr<PhysicalAddress>::create(PhysicalAddress(entry->get_ppn() | address.offset));
    }

    entry = &reinterpret_cast<PageTableEntry*>(entry->get_ppn())[address.vpn1];

    if(!entry->is_valid()) {
      return ErrorOr<PhysicalAddress>::create_error(Error::create_from_string("Second entry is not valid"));
    }

    if(entry->is_leaf()) {
      return ErrorOr<PhysicalAddress>::create(PhysicalAddress(entry->get_ppn() | address.offset));
    }

    entry = &reinterpret_cast<PageTableEntry*>(entry->get_ppn())[address.vpn0];
    if(!entry->is_valid()) {
      return ErrorOr<PhysicalAddress>::create_error(Error::create_from_string("Third entry is not valid"));
    }

    return ErrorOr<PhysicalAddress>::create(PhysicalAddress(entry->get_ppn() | address.offset));
  }
};// namespace Kernel