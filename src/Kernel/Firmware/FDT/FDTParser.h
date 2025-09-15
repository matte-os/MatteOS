/**
 * @file FDTParser.h
 * @author Matěj Bucek
 */
#pragma once

#include <Kernel/Firmware/FDT/FDT.h>
#include <Utils/Arrays/ArrayList.h>
#include <Utils/DebugConsole.h>
#include <Utils/Errors/ErrorOr.h>

using Utils::ArrayList;
using Utils::DebugConsole;
using Utils::Error;
using Utils::ErrorOr;
using Utils::move;

namespace Kernel {
  class FDTParser;

  struct FDTProperty {
    friend class FDTParser;

  private:
    String m_name;
    String m_value;

  public:
    FDTProperty() = default;
    FDTProperty(String name, String value) : m_name(move(name)), m_value(move(value)) {}

    [[nodiscard]] String get_name() const {
      return m_name;
    }

    [[nodiscard]] String get_value() const {
      return m_value;
    }

    [[nodiscard]] String get_value_as_string() const {
      return String(m_value.to_cstring());
    }

    char* get_value_bytes() const {
      return const_cast<char*>(m_value.to_cstring());
    }

    u32 get_value_as_u32(size_t index = 0) const {
      return *(reinterpret_cast<Endian<u32, Utils::Endianness::Big>*>(get_value_bytes())[index]);
    }

    u32 number_of_u32_values() const {
      return m_value.length() / sizeof(u32);
    }

    [[nodiscard]] bool is_empty() const {
      return m_value.length() == 0;
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

    ErrorOr<const FDTProperty*> find_property(const String& name) const {
      for(size_t i = 0; i < m_properties.size(); i++) {
        if(m_properties[i].get_name() == name) {
          return ErrorOr<const FDTProperty*>::create(&m_properties[i]);
        }
      }
      return ErrorOr<const FDTProperty*>::create_error(Error::create_from_string("Property not found"));
    }

    [[nodiscard]] const ArrayList<FDTProperty>& get_properties() const {
      return m_properties;
    }

    [[nodiscard]] const ArrayList<FDTNode*>& get_children() const {
      return m_children;
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

    void print_reserve_entries() {
      DebugConsole::println("Reserve entries:");
      for(size_t i = 0; i < m_reserve_entries.size(); i++) {
        DebugConsole::print("Address: ");
        DebugConsole::print_ln_number(*m_reserve_entries[i].address, 16);
        DebugConsole::print("Size: ");
        DebugConsole::print_ln_number(*m_reserve_entries[i].size, 16);
      }
    }
  };
}// namespace Kernel
