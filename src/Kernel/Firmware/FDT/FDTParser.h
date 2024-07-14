#pragma once

#include <Kernel/Firmware/FDT/fdt.h>
#include <Utils/Arrays/ArrayList.h>
#include <Utils/DebugConsole.h>
#include <Utils/Errors/ErrorOr.h>

using Utils::ArrayList;
using Utils::DebugConsole;
using Utils::ErrorOr;
using Utils::move;

namespace Kernel::Firmware::FDT {
  class FDTParser;
  struct FDTProperty {
    friend class FDTParser;

  private:
    String m_name;
    String m_value;

  public:
    FDTProperty(String name, String value) : m_name(move(name)), m_value(move(value)) {}

    [[nodiscard]] String get_name() const {
      return m_name;
    }

    [[nodiscard]] String get_value() const {
      return m_value;
    }
    void print() const {
      DebugConsole::print(m_name.to_cstring());
      DebugConsole::print(": ");
      DebugConsole::print(m_value.to_cstring());
      DebugConsole::print("\n");
    }
  };

  struct FDTNode {
    friend class FDTParser;

  private:
    String m_name;
    ArrayList<FDTProperty> m_properties;
    ArrayList<FDTNode*> m_children;

  public:
    void print() {
      DebugConsole::print(m_name.to_cstring());
      DebugConsole::print("\n");
      DebugConsole::print("Number of properties: ");
      DebugConsole::print_ln_number(m_properties.size(), 10);
      for(size_t i = 0; i < m_properties.size(); i++) {
        m_properties.get(i).print();
      }

      DebugConsole::print("Number of children: ");
      DebugConsole::print_ln_number(m_children.size(), 10);
      for(size_t i = 0; i < m_children.size(); i++) {
        m_children.get(i)->print();
      }
      DebugConsole::println("Children end");
    }

    String get_name() const {
      auto splitted = m_name.split("@");
      return splitted[0];
    }

    String get_address() const {
      auto splitted = m_name.split("@");
      if(splitted.size() == 1) {
        return "";
      }
      return splitted[1];
    }

    String get_full_name() const {
      return m_name;
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
    uintptr_t align_to(u32 alignment, uintptr_t address);

    template<typename T>
    T* access_at_offset(u32 offset) {
      return reinterpret_cast<T*>(reinterpret_cast<char*>(m_header) + offset);
    }

  public:
    void parse(FDTHeader* header);
    [[nodiscard]] const ArrayList<FDTReserveEntry>& get_reserve_entries() const;
    [[nodiscard]] const FDTNode& get_root_node() const;
    [[nodiscard]] ErrorOr<const FDTNode*> find_node(const String& path) const;
    [[nodiscard]] ErrorOr<ArrayList<const FDTNode*>> find_nodes(const String& path) const;
    void print_debug() {
      m_root_node->print();
    }
  };
}// namespace Kernel::Firmware::FDT
