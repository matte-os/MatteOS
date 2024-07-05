//
// Created by matejbucek on 4.7.24.
//

#include <Kernel/VirtIO/MMIODevice.h>
#include <Kernel/System/DeviceManager.h>
#include <Kernel/Memory/MemoryManager.h>
#include <Utils/DebugConsole.h>

namespace Kernel {
  using Utils::DebugConsole;
  using Kernel::Memory::EntryBits;
  using Kernel::Memory::MemoryManager;

  static DeviceManager* g_device_manager = nullptr;

  void DeviceManager::init() {
    if(g_device_manager == nullptr) {
      g_device_manager = new DeviceManager;
    } else {
      DebugConsole::println("DeviceManager: Already initialised.");
    }
  }

  DeviceManager& DeviceManager::the() {
    return *g_device_manager;
  }
  ErrorOr<RefPtr<Device>> DeviceManager::try_to_load_mmio_device(uintptr_t address) {
    DebugConsole::print("DeviceManager: Trying to load MMIO device at address ");
    DebugConsole::print_number(address, 16);
    DebugConsole::println(".");

    DebugConsole::println("DeviceManager: Mapping MMIO device.");
    auto result = MemoryManager::the().get_current_root_page_table();
    MemoryManager::the().identity_map_range(*result, address, address + 0x1000, (u64) EntryBits::READ_WRITE);

    auto mmio_device = reinterpret_cast<MMIODevice*>(address);
    if(mmio_device->get_magic_value() == 0x74726976) {
      DebugConsole::println("DeviceManager: Found VirtIO device.");
      auto virtio_device = RefPtr<UnderlyingDevice>(new VirtIODevice(mmio_device));
      auto device = RefPtr<Device>(new Device(virtio_device));
      return ErrorOr<RefPtr<Device>>::create(move(device));
    } else {
      DebugConsole::println("DeviceManager: Not a VirtIO device.");
      return ErrorOr<RefPtr<Device>>::create_error(Error::create_from_string("Not a VirtIO device."));
    }
  }
}// namespace Kernel