//
// Created by matejbucek on 4.7.24.
//

#include <Kernel/Devices/BlockDevice.h>
#include <Kernel/Devices/Device.h>
#include <Kernel/Devices/DeviceManager.h>
#include <Kernel/Devices/EntropyDevice.h>
#include <Kernel/Drivers/DriverManager.h>
#include <Kernel/Drivers/VirtIO/MMIODevice.h>
#include <Kernel/Drivers/VirtIO/VirtIODeviceIDs.h>
#include <Kernel/Memory/MemoryManager.h>
#include <Utils/Assertions.h>
#include <Utils/DebugConsole.h>
#include <Utils/Traits.h>

namespace Kernel {
  using Kernel::EntryBits;
  using Kernel::get_device_type;
  using Kernel::MemoryManager;
  using Utils::as_underlying;
  using Utils::DebugConsole;
  using Utils::Error;
  using Utils::ErrorOr;

  static DeviceManager* s_device_manager = nullptr;

  void DeviceManager::init() {
    if(s_device_manager == nullptr) {
      s_device_manager = new DeviceManager;
    } else {
      DebugConsole::println("DeviceManager: Already initialised.");
    }
  }

  DeviceManager& DeviceManager::the() {
    runtime_assert(s_device_manager, "DeviceManager is not initialised.");
    return *s_device_manager;
  }

  ErrorOr<RefPtr<Device>> DeviceManager::try_to_load_mmio_device(uintptr_t address, ArrayList<u64>&& interrupts) {
    DebugConsole::print("DeviceManager: Trying to load MMIO device at address ");
    DebugConsole::print_number(address, 16);
    DebugConsole::println(".");

    DebugConsole::println("DeviceManager: Mapping MMIO device.");
    auto result = MemoryManager::the().get_current_root_page_table();
    MemoryManager::the().identity_map_range(*result, address, address + 0x1000, (u64) EntryBits::READ_WRITE);

    auto mmio_device = reinterpret_cast<MMIODevice*>(address);
    if(mmio_device->get_magic_value() == 0x74726976) {
      DebugConsole::println("DeviceManager: Found VirtIO device.");
      DebugConsole::print("DeviceManager: The VirtIO version is ");
      DebugConsole::print_ln_number(mmio_device->get_version(), 10);
      auto virtio_device = RefPtr<UnderlyingDevice>(new VirtIODevice(mmio_device));

      RefPtr<Device> device;
      switch(get_device_type(virtio_device->get_device_id())) {
        case VirtIODeviceIDs::EntropySource:
          device = RefPtr<Device>(new EntropyDevice(virtio_device, move(interrupts)));
          break;
        case VirtIODeviceIDs::BlockDevice:
          device = RefPtr<Device>(new BlockDevice(virtio_device, move(interrupts)));
          break;
        default:
          device = RefPtr<Device>(new Device(virtio_device, move(interrupts)));
      }
      m_devices.add(device);
      return ErrorOr<RefPtr<Device>>::create(move(device));
    } else {
      DebugConsole::println("DeviceManager: Not a VirtIO device.");
      return ErrorOr<RefPtr<Device>>::create_error(Error::create_from_string("Not a VirtIO device."));
    }
  }

  ErrorOr<void> DeviceManager::add_device(const RefPtr<Device>& device) {
    m_devices.add(device);
    return ErrorOr<void>::create({});
  }

  ErrorOr<void> DeviceManager::delegate_device_interrupt(u64 interrupt_id) {
    auto device_or_error = m_devices.find_first_match([interrupt_id](const RefPtr<Device>& device) -> bool {
      return device->handles_interrupt(interrupt_id);
    });

    if(device_or_error.has_error()) {
      return ErrorOr<void>::create_error(Error::create_from_string("Couldn't delegate interrupt. No device found!."));
    }

    auto device = device_or_error.get_value();
    return device->handle_interrupt(interrupt_id);
  }

  void DeviceManager::load_drivers() {
    for(size_t i = 0; i < m_devices.size(); i++) {
      auto device = m_devices[i];
      if(device->has_driver() || !device->needs_driver()) {
        continue;
      }

      auto drivers = DriverManager::the().find_compatible_drivers(device);
      if(drivers.size() > 0) {
        auto driver_or_error = drivers[0]->instantiate_driver(device);
        if(driver_or_error.has_error()) {
          DebugConsole::println("DeviceManager: Could not instantiate driver.");
        } else {
          auto driver = driver_or_error.get_value();
          device->set_driver(driver);
          driver->init(device);
          DebugConsole::println("DeviceManager: Driver loaded.");
        }
      }
    }
  }

  ArrayList<RefPtr<Device>> DeviceManager::get_devices_of_type(DeviceType type) {
    return m_devices.find_all_matches([type](const RefPtr<Device>& device) -> bool {
      return device->get_device_type() == type;
    });
  }
}// namespace Kernel