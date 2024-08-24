//
// Created by matejbucek on 6.7.24.
//

#include <Kernel/Drivers/DriverManager.h>
#include <Kernel/Drivers/VirtIO/BlockIODriver.h>
#include <Kernel/Devices/Device.h>
#include <Utils/Assertions.h>
#include <Utils/DebugConsole.h>
#include <Utils/Errors/ErrorOr.h>

using Utils::DebugConsole;

namespace Kernel {
  DriverManager* s_driver_manager = nullptr;

  using Utils::as_underlying;
  using Utils::Error;

  void DriverManager::init() {
    if(s_driver_manager == nullptr) {
      s_driver_manager = new DriverManager;
    } else {
      DebugConsole::println("DriverManager: Already initialised.");
    }

    s_driver_manager->register_default_drivers();
  }

  DriverManager& Kernel::DriverManager::the() {
    runtime_assert(s_driver_manager, "DriverManager: Not initialised.");
    return *s_driver_manager;
  }

  void DriverManager::register_default_drivers() {
    register_driver(RefPtr<DriverDescriptor>(new VirtIODriverDescriptor()));
  }

  void DriverManager::register_driver(RefPtr<DriverDescriptor> driver_descriptor) {
    m_driver_descriptors.add(move(driver_descriptor));
    DebugConsole::println("DriverManager: Registered driver.");
  }

  ArrayList<RefPtr<DriverDescriptor>> DriverManager::find_compatible_drivers(RefPtr<Device> device) {
    return m_driver_descriptors.find_all_matches([device](auto descriptor) -> bool {
      return descriptor->is_compatible_with(device);
    });
  }
}// namespace Kernel
