//
// Created by matejbucek on 4.7.24.
//

#include <Kernel/Memory/MemoryManager.h>
#include <Kernel/System/DeviceManager.h>
#include <Kernel/VirtIO/MMIODevice.h>
#include <Kernel/VirtIO/VirtIODeviceIDs.h>
#include <Utils/Assertions.h>
#include <Utils/DebugConsole.h>

namespace Kernel {
  using Kernel::get_device_type;
  using Kernel::Memory::EntryBits;
  using Kernel::Memory::MemoryManager;
  using Utils::DebugConsole;

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
      DebugConsole::print("DeviceManager: The VirtIO version is ");
      DebugConsole::print_ln_number(mmio_device->get_version(), 10);
      auto virtio_device = RefPtr<UnderlyingDevice>(new VirtIODevice(mmio_device));
      RefPtr<Device> device;
      switch(get_device_type(virtio_device->get_device_id())) {
        case VirtIODeviceIDs::EntropySource:
          device = RefPtr<Device>(new EntropyDevice(virtio_device));
          break;
        default:
          device = RefPtr<Device>(new Device(virtio_device));
      }
      m_devices.add(device);
      return ErrorOr<RefPtr<Device>>::create(move(device));
    } else {
      DebugConsole::println("DeviceManager: Not a VirtIO device.");
      return ErrorOr<RefPtr<Device>>::create_error(Error::create_from_string("Not a VirtIO device."));
    }
  }
  ErrorOr<void> EntropyDevice::init() {
    return m_underlying_device->init(0, [](VirtQueue*) {
      DebugConsole::println("EntropyDevice: Initialising VirtQueue.");
    });
  }
  ErrorOr<void> Device::init() {
    return ErrorOr<void>::create_error(Error::create_from_string("Device slot is empty."));
  }
  // https://docs.oasis-open.org/virtio/virtio/v1.2/csd01/virtio-v1.2-csd01.html#x1-1070001
  // 3.1.1 Driver Requirements: Device Initialization
  ErrorOr<void> VirtIODevice::init(u32 features, Function<void, VirtQueue*> init_virt_queue) {
    // The driver MUST follow this sequence to initialize the device:
    // 1. Reset the device.
    reset();
    // 2. Set the ACKNOWLEDGE status bit: the guest OS has noticed the device.
    auto status = m_mmio_device->get_status() | static_cast<u32>(StatusField::Acknowledge);
    m_mmio_device->set_status(status);
    // 3. Set the DRIVER status bit: the guest OS knows how to drive the device.
    status |= static_cast<u32>(StatusField::Driver);
    m_mmio_device->set_status(status);
    // 4. Read device feature bits, and write the subset of feature bits understood by the OS and driver to the device.
    // During this step the driver MAY read (but MUST NOT write) the device-specific configuration fields to check that
    // it can support the device before accepting it.
    auto host_features = m_mmio_device->get_host_features();
    if((host_features & features) != features) {
      return ErrorOr<void>::create_error(Error::create_from_string("Device does not support required features."));
    }
    m_mmio_device->set_guest_features(features);
    // 5. Set the FEATURES_OK status bit. The driver MUST NOT accept new feature bits after this step.
    status |= static_cast<u32>(StatusField::FeaturesOK);
    m_mmio_device->set_status(status);
    // 6. Re-read device status to ensure the FEATURES_OK bit is still set: otherwise,
    // the device does not support our subset of features and the device is unusable.
    status = m_mmio_device->get_status();
    if((status & static_cast<u32>(StatusField::FeaturesOK)) == 0) {
      return ErrorOr<void>::create_error(Error::create_from_string("Device does not support required features."));
    }
    // 7. Perform device-specific setup, including discovery of virtqueues for the device, optional per-bus setup,
    // reading and possibly writing the device’s virtio configuration space, and population of virtqueues.
    auto queue_num_max = m_mmio_device->get_queue_num_max();
    m_mmio_device->set_queue_num(VIRTIO_RING_SIZE);
    if(VIRTIO_RING_SIZE > queue_num_max) {
      return ErrorOr<void>::create_error(Error::create_from_string("Device does not support required number of queues."));
    }

    m_mmio_device->set_queue_sel(0);

    auto number_of_pages = (sizeof(VirtQueue) + PAGE_SIZE - 1) / PAGE_SIZE;
    auto queue_address = MemoryManager::the().zalloc(number_of_pages);
    m_mmio_device->set_guest_page_size(PAGE_SIZE);
    // Store the address of the VirtQueue for quick access.
    m_virt_queue = reinterpret_cast<VirtQueue*>(queue_address);
    // Set the address of the VirtQueue in the device.
    auto queue_address_value = reinterpret_cast<size_t>(queue_address);
    m_mmio_device->set_queue_pfn(static_cast<u32>(queue_address_value) / PAGE_SIZE);

    // Callback to the driver to populate the VirtQueue.
    init_virt_queue(m_virt_queue);

    // 8. Set the DRIVER_OK status bit. At this point the device is “live”.
    status |= static_cast<u32>(StatusField::DriverOK);
    m_mmio_device->set_status(status);
    return ErrorOr<void>::create({});
  }
  void VirtIODevice::reset() {
    m_mmio_device->set_status(0);
  }
}// namespace Kernel