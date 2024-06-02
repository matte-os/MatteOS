#pragma once

#include <Kernel/Firmware/FDT/fdt.h>
#include <Utils/Arrays/ArrayList.h>
#include <Utils/DebugConsole.h>

using Utils::ArrayList;
using Utils::DebugConsole;

namespace Kernel::Firmware::FDT {
  struct FDTProperty {
    String name;
    String value;

    void print() const {
      DebugConsole::print(name.to_cstring());
      DebugConsole::print(": ");
      DebugConsole::print(value.to_cstring());
      DebugConsole::print("\n");
    }
  };

  struct FDTNode {
    String name;
    ArrayList<FDTProperty> properties;
    ArrayList<FDTNode*> children;

    void print() {
      DebugConsole::print(name.to_cstring());
      DebugConsole::print("\n");
      DebugConsole::print("Number of properties: ");
      DebugConsole::print_ln_number(properties.size(), 10);
      for(size_t i = 0; i < properties.size(); i++) {
        properties.get(i).print();
      }

      DebugConsole::print("Number of children: ");
      DebugConsole::print_ln_number(children.size(), 10);
      for(size_t i = 0; i < children.size(); i++) {
        children.get(i)->print();
      }
      DebugConsole::println("Children end");
    }
  };

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
