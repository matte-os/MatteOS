//
// Created by matejbucek on 6.7.24.
//

#include <Kernel/System/DeviceManager.h>
#include <Kernel/System/DriverManager.h>
#include <Kernel/VirtIO/BlockIO.h>
#include <Kernel/VirtIO/VirtQueue.h>
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

  ErrorOr<void> BlockIODriver::block_operation(u8* buffer, u64 size, u64 offset, bool is_write) {
    auto* request = new BlockIO::Request {
            .header = BlockIO::Header {
                    .type = is_write ? as_underlying(BlockIO::RequestType::Write) : as_underlying(BlockIO::RequestType::Read),
                    .reserved = 0,
                    .sector = offset / 512},
            .data = buffer,
            .status = 111};

    // The first VirtQueue descriptor that will tell the drive
    // that we want to read/write data from/to the disk.
    auto descriptor = VirtQueueDescriptor {
            .address = reinterpret_cast<u64>(&request->header),
            .length = sizeof(BlockIO::Header),
            .flags = as_underlying(VirtQueueDescriptorFlags::Next),
            .next = 0};

    // Move the descriptor to the VirtQueue and link next.
    auto head_or_error = virtio_device()->add_to_queue(move(descriptor));

    // The second VirtQueue descriptor that will tell the drive
    // where to store/load the data.
    descriptor = VirtQueueDescriptor {
            .address = reinterpret_cast<u64>(buffer),
            .length = size,
            .flags = as_underlying(VirtQueueDescriptorFlags::Next) | (is_write ? as_underlying(VirtQueueDescriptorFlags::Write) : 0),
            .next = 0};

    // Move the descriptor to the VirtQueue and link next.
    virtio_device()->add_to_queue(move(descriptor));

    // The third descriptor that will tell the drive how much data to load.
    descriptor = VirtQueueDescriptor {
            .address = reinterpret_cast<u64>(&request->status),
            .length = sizeof(request->status),
            .flags = as_underlying(VirtQueueDescriptorFlags::Next),
            .next = 0};

    // Move the descriptor to the VirtQueue.
    virtio_device()->add_to_queue(move(descriptor));

    if(head_or_error.has_error()) {
      return ErrorOr<void>::create_error(head_or_error.get_error());
    }

    auto head = head_or_error.get_value();
    virtio_device()->add_to_available(head);
    virtio_device()->notify();

    return ErrorOr<void>::create({});
  }

  BlockIODriver::BlockIODriver(RefPtr<BlockDevice> device) {
    m_device = move(device);
  }

  void BlockIODriver::init(RefPtr<Device> device) {
  }

  void BlockIODriver::shutdown() {
  }

  ErrorOr<void> BlockIODriver::write(u8* buffer, u64 size, u64 offset) {
    return block_operation(buffer, size, offset, true);
  }

  ErrorOr<void> BlockIODriver::read(u8* buffer, u64 size, u64 offset) {
    return block_operation(buffer, size, offset, false);
  }

  RefPtr<VirtIODevice> BlockIODriver::virtio_device() const {
    return m_device->get_underlying_device();
  }

  bool VirtIODriverDescriptor::is_compatible_with(RefPtr<Device> device) {
    if(device->get_underlying_device()->get_device_type() != UnderlyingDeviceType::VirtIO) {
      return false;
    }

    if(device->get_device_type() == DeviceType::Block) {
      return true;
    }

    return false;
  }

  ErrorOr<RefPtr<Driver>> VirtIODriverDescriptor::instantiate_driver(RefPtr<Device> device) {
    if(!is_compatible_with(device)) {
      return ErrorOr<RefPtr<Driver>>::create_error(Error::create_from_string("Device is not compatible."));
    }

    if(device->get_device_type() == DeviceType::Block) {
      return ErrorOr<RefPtr<Driver>>::create(RefPtr<Driver>(new BlockIODriver(device)));
    }

    return ErrorOr<RefPtr<Driver>>::create_error(Error::create_from_string("Device is not compatible. Driver could not be instantiated."));
  }
}// namespace Kernel
