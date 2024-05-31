#pragma once

#include <Kernel/Firmware/FDT/fdt.h>
#include <Utils/Arrays/ArrayList.h>

using Utils::ArrayList;

namespace Kernel::Firmware::FDT {
  class FDTParser {
  private:
    FDTHeader* m_header;
    ArrayList<FDTReserveEntry> m_reserve_entries;
    FDTNode* m_root_node;

    void parse_reserve_entries();
    void parse_nodes();
    FDTNode* parse_node(u32** current_offset);
    uintptr_t aling_to(u32 alignment, uintptr_t address);

    template<typename T>
    T* access_at_offset(u32 offset) {
      return reinterpret_cast<T*>(reinterpret_cast<char*>(m_header) + offset);
    }

  public:
    void parse(FDTHeader* header);
    [[nodiscard]] const ArrayList<FDTReserveEntry>& get_reserve_entries() const;
    [[nodiscard]] const FDTNode& get_root_node() const;
  };
}// namespace Kernel::Firmware::FDT
