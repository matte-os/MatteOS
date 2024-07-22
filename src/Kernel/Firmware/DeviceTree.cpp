#include <Kernel/Firmware/DeviceTree.h>
#include <Utils/Assertions.h>

namespace Kernel {
  static DeviceTree* s_device_tree = nullptr;

  void DeviceTree::init(FDTHeader* header) {
    if(s_device_tree) {
      return;
    }
    s_device_tree = new DeviceTree();
    if(*header->magic == 0xd00dfeed) {
      s_device_tree->m_header = header;

      auto fdt_version = *header->version;
      auto total_size = *header->totalsize;

      Utils::DebugConsole::print("System: FDT Version ");
      Utils::DebugConsole::print_number(fdt_version, 10);
      Utils::DebugConsole::print(" Total Size ");
      Utils::DebugConsole::print_number(total_size, 10);
      Utils::DebugConsole::println(".");
      s_device_tree->m_parser = new FDTParser();
      s_device_tree->m_parser->parse(header);

      s_device_tree->m_parser->print_reserve_entries();
    } else {
      runtime_assert(false, "Invalid FDT header");
    }
  }

  DeviceTree& DeviceTree::the() {
    runtime_assert(s_device_tree, "DeviceTree is not initialized");
    return *s_device_tree;
  }


}// namespace Kernel