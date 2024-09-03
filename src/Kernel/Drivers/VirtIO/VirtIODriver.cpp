//
// Created by matejbucek on 3.9.24.
//

#include <Kernel/Devices/DeviceManager.h>
#include <Kernel/Drivers/VirtIO/VirtIODriver.h>
#include <Kernel/Firmware/DeviceTree.h>

namespace Kernel {
  void VirtIODriver::init() {
    DebugConsole::println("VirtIODriver: The driver will try to initialize the VirtIO devices.");
    auto error_or_nodes = DeviceTree::the().find_nodes("/soc/virtio_mmio");
    if(error_or_nodes.has_error()) {
      DebugConsole::println("VirtIODriver: Could not find /soc/virtio_mmio node in FDT.");
      return;
    }

    auto nodes = error_or_nodes.get_value();
    for(auto* node: nodes) {
      auto string_address = node->get_address();
      auto address = string_address.to_uint(16);

      ArrayList<u64> interrupts;
      auto node_interrupts_or_error = node->find_property("interrupts");
      if(node_interrupts_or_error.has_value()) {
        auto node_interrupts = node_interrupts_or_error.get_value();
        for(size_t j = 0; j < node_interrupts->number_of_u32_values(); j++) {
          interrupts.add(node_interrupts->get_value_as_u32(j));
        }
      }

      auto result = DeviceManager::the().try_to_load_mmio_device(address, move(interrupts));
      if(result.has_error()) {
        DebugConsole::println("System: Could not load MMIO device.");
      } else {
        DebugConsole::println("System: Loaded MMIO device.");
        DebugConsole::print("System: The device type is: ");
        DebugConsole::print_ln_number(static_cast<size_t>(result.get_value()->get_device_type()), 10);
        auto init_or_error = result.get_value()->init();
        if(init_or_error.has_error()) {
          DebugConsole::print("System: Could not initialise device. Error: ");
          DebugConsole::println(init_or_error.get_error().get_message().value());
        } else {
          DebugConsole::println("System: Initialised device.");
        }
      }
    }

    DebugConsole::println("VirtIODriver: The driver has initialized the VirtIO devices.");
  }

  void VirtIODriver::shutdown() {
    DebugConsole::println("VirtIODriver: The driver will try to shutdown the VirtIO devices.");
  }
}// namespace Kernel