#include <Kernel/Firmware/FDT/FDTParser.h>
#include <Utils/DebugConsole.h>
#include <Utils/Utility.h>

using Kernel::Firmware::FDT::FDTNode;
using Kernel::Firmware::FDT::FDTProperty;
using Utils::DebugConsole;
using Utils::move;
using Utils::Errors::Error;
using Utils::Errors::ErrorOr;

void Kernel::Firmware::FDT::FDTParser::parse_reserve_entries() {
  DebugConsole::println("Parsing reserve entries");
  auto* reserve_entry = access_at_offset<FDTReserveEntry>(*m_header->off_mem_rsvmap);
  while(*reserve_entry->address != 0 && *reserve_entry->size != 0) {
    DebugConsole::print("Reserve entry: ");
    DebugConsole::print_number(*reserve_entry->address, 16);
    DebugConsole::print(" ");
    DebugConsole::print_number(*reserve_entry->size, 16);
    DebugConsole::println("");
    m_reserve_entries.add(*reserve_entry);
    reserve_entry++;
  }
  DebugConsole::println("Finished parsing reserve entries");
}

void Kernel::Firmware::FDT::FDTParser::parse_nodes() {
  DebugConsole::println("Parsing nodes");
  u32* current_offset = reinterpret_cast<u32*>(reinterpret_cast<char*>(m_header) + *m_header->off_dt_struct);
  auto* node = parse_node(&current_offset);
  if(node == nullptr) {
    DebugConsole::println("Failed to parse root node");
    return;
  }

  m_root_node = node;
  DebugConsole::println("Finished parsing nodes");
}

FDTNode* Kernel::Firmware::FDT::FDTParser::parse_node(u32** offset) {
  enum class ParserState {
    None,
    BeginNode,
    NodeName,
    EndNode,
    Prop,
    End
  } state = ParserState::None;

  auto* current_node = new FDTNode();

  auto* current_offset = *offset;

  while(reinterpret_cast<uintptr_t>(current_offset) - reinterpret_cast<uintptr_t>(m_header) < *m_header->size_dt_struct) {
    auto type = __builtin_bswap32(*current_offset);
    bool skip_increment = false;

    switch(state) {
      case ParserState::None: {
        if(type == static_cast<u32>(FDTNodeType::FDT_BEGIN_NODE)) {
          state = ParserState::BeginNode;
        } else {
          delete current_node;
          return nullptr;
        }
      } break;
      case ParserState::BeginNode: {
        current_node->m_name = String(reinterpret_cast<char*>(current_offset));
        current_offset = reinterpret_cast<u32*>(reinterpret_cast<char*>(current_offset) + current_node->m_name.length());
        current_offset = reinterpret_cast<u32*>(align_to(4, reinterpret_cast<uintptr_t>(current_offset)));
        skip_increment = true;
        state = ParserState::NodeName;
      } break;
      case ParserState::NodeName: {
        if(type == 0 || type == static_cast<u32>(FDTNodeType::FDT_NOP)) {
          current_offset++;
          continue;
        } else if(type == static_cast<u32>(FDTNodeType::FDT_PROP)) {
          state = ParserState::Prop;
        } else if(type == static_cast<u32>(FDTNodeType::FDT_BEGIN_NODE)) {
          auto* child_node = parse_node(&current_offset);
          if(child_node != nullptr) {
            current_node->m_children.add(child_node);
          }
        } else if(type == static_cast<u32>(FDTNodeType::FDT_END_NODE)) {
          state = ParserState::EndNode;
          skip_increment = true;
        } else if(type == static_cast<u32>(FDTNodeType::FDT_END)) {
          state = ParserState::End;
        } else {
          DebugConsole::print("Unknown node type: ");
          DebugConsole::print_number(type, 16);
          delete current_node;
          return nullptr;
        }
      } break;
      case ParserState::Prop: {
        auto* prop = reinterpret_cast<FDTProp*>(current_offset);
        current_offset = reinterpret_cast<u32*>(reinterpret_cast<char*>(current_offset) + sizeof(FDTProp));
        auto name = String(reinterpret_cast<char*>(m_header) + static_cast<u64>(*m_header->off_dt_strings) + static_cast<u64>(*prop->name_offset));
        auto value = String(reinterpret_cast<char*>(current_offset), *prop->len);
        current_node->m_properties.add({move(name), move(value)});
        current_offset = reinterpret_cast<u32*>(reinterpret_cast<char*>(current_offset) + *prop->len);
        current_offset = reinterpret_cast<u32*>(align_to(4, reinterpret_cast<uintptr_t>(current_offset)));
        skip_increment = true;
        state = ParserState::NodeName;
      } break;
      case ParserState::EndNode: {
        *offset = current_offset;
        return current_node;
      }
    }

    if(!skip_increment) {
      current_offset++;
    }
  }
  *offset = current_offset;
  return current_node;
}

void Kernel::Firmware::FDT::FDTParser::parse(FDTHeader* header) {
  m_header = header;
  parse_reserve_entries();
  parse_nodes();
}

const ArrayList<FDTReserveEntry>& Kernel::Firmware::FDT::FDTParser::get_reserve_entries() const {
  return m_reserve_entries;
}

uintptr_t Kernel::Firmware::FDT::FDTParser::align_to(u32 alignment, uintptr_t address) {
  return (address + alignment - 1) & ~(alignment - 1);
}
const FDTNode& Kernel::Firmware::FDT::FDTParser::get_root_node() const {
  return *m_root_node;
}
ErrorOr<const FDTNode*> Kernel::Firmware::FDT::FDTParser::find_node(const String& path) const {
  FDTNode* current_node = m_root_node;
  auto path_array = path.split("/");
  size_t i = 0, path_array_index = 0;
  while(current_node != nullptr && i < current_node->m_children.size() && path_array_index < path_array.size()) {
    if(current_node->m_children[i] == nullptr) {
      continue;
    }

    if(current_node->m_children[i]->get_name() == path_array[path_array_index]) {
      current_node = current_node->m_children[i];
      i = 0;
      path_array_index++;
      continue;
    }

    i++;
  }

  if(path_array_index == path_array.size()) {
    return ErrorOr<const FDTNode*>::create(move(current_node));
  } else {
    return ErrorOr<const FDTNode*>::create_error(Error::create_from_string(String("Node not found")));
  }
}
ErrorOr<ArrayList<const FDTNode*>> Kernel::Firmware::FDT::FDTParser::find_nodes(const String& path) const {
  FDTNode* current_node = m_root_node;
  auto path_array = path.split("/");
  size_t i = 0, path_array_index = 0;
  ArrayList<const FDTNode*> nodes;
  while(current_node != nullptr && i < current_node->m_children.size() && path_array_index < path_array.size()) {
    if(current_node->m_children[i] == nullptr) {
      continue;
    }

    if(current_node->m_children[i]->get_name() == path_array[path_array_index]) {
      if(path_array_index == path_array.size() - 1) {
        nodes.add(current_node->m_children[i]);
      } else {
        current_node = current_node->m_children[i];
        i = 0;
        path_array_index++;
        continue;
      }
    }

    i++;
  }

  return ErrorOr<ArrayList<const FDTNode*>>::create(move(nodes));
}
