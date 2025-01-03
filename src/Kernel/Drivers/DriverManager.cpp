//
// Created by matejbucek on 6.7.24.
//

#include <Kernel/Devices/Device.h>
#include <Kernel/Drivers/DriverManager.h>
#include <Kernel/Drivers/Storage/VirtIO/BlockIODriver.h>
#include <Kernel/Drivers/VirtIO/VirtIODriver.h>
#include <Kernel/Drivers/Console/NS16550ADriver.h>
#include <Kernel/Drivers/MMIODriver.h>
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
      return;
    }

    s_driver_manager->register_default_drivers();
  }

  DriverManager& Kernel::DriverManager::the() {
    runtime_assert(s_driver_manager, "DriverManager: Not initialised.");
    return *s_driver_manager;
  }

  void DriverManager::register_default_drivers() {
    register_device_driver(RefPtr<DeviceDriverDescriptor>(new VirtIODeviceDriverDescriptor()));
    register_device_driver(RefPtr<DeviceDriverDescriptor>(new NS16550ADriverDescriptor()));
    register_driver(RefPtr<DriverDescriptor>(new VirtIODriverDescriptor()));
    register_driver(RefPtr<DriverDescriptor>(new MMIODriverDescriptor()));
  }

  void DriverManager::register_device_driver(RefPtr<DeviceDriverDescriptor> driver_descriptor) {
    m_device_driver_descriptors.add(move(driver_descriptor));
    DebugConsole::println("DriverManager: Registered device driver.");
  }

  ArrayList<RefPtr<DeviceDriverDescriptor>> DriverManager::find_compatible_drivers(RefPtr<Device> device) {
    return m_device_driver_descriptors.find_all_matches([device](auto descriptor) -> bool {
      return descriptor->is_compatible_with(device);
    });
  }

  void DriverManager::register_driver(RefPtr<DriverDescriptor> driver_descriptor) {
    m_driver_descriptors.add(move(driver_descriptor));
    DebugConsole::println("DriverManager: Registered driver.");
  }

  void DriverManager::init_drivers() {
    for(auto& driver_descriptor: m_driver_descriptors) {
      auto driver_or_error = driver_descriptor->instantiate_driver();
      if(driver_or_error.has_error()) {
        DebugConsole::println("DriverManager: Failed to instantiate driver.");
        continue;
      }
      auto driver = driver_or_error.get_value();
      driver->init();
      m_drivers.add(move(driver));
    }
  }
}// namespace Kernel
