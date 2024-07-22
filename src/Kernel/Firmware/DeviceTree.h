#pragma once

#include <Kernel/Firmware/FDT/FDT.h>
#include <Kernel/Firmware/FDT/FDTParser.h>

namespace Kernel {

  class DeviceTree {
  private:
    FDTHeader* m_header;
    FDTParser* m_parser;

    DeviceTree() = default;

  public:
    static void init(FDTHeader* header);
    static DeviceTree& the();
    const FDTHeader& get_header() const { return *m_header; }
    const FDTNode& get_root_node() const { return m_parser->get_root_node(); }
    ErrorOr<const FDTNode*> find_node(const String& path) const { return m_parser->find_node(path); }
    ErrorOr<ArrayList<const FDTNode*>> find_nodes(const String& path) const { return m_parser->find_nodes(path); }
  };

}// namespace Kernel
