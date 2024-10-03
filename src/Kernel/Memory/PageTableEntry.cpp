#include <Kernel/Memory/PageTableEntry.h>

namespace Kernel {
  u64 PageTableEntry::get_value() {
    u64 out = *(u64*) this;
    return out;
  }

  void PageTableEntry::set_value(u64 value) {
    *(u64*) this = value;
  }

  void PageTableEntry::set_bits(u64 bits) {
    set_value(get_value() | bits);
  }

  bool PageTableEntry::is_valid() {
    return valid == 1;
  }

  bool PageTableEntry::is_leaf() {
    return read || write || execute;
  }

  uintptr_t PageTableEntry::get_ppn() {
    return ((get_value() & ~0x1ff) << 2);
  }
};// namespace Kernel
