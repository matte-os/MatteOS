//
// Created by matejbucek on 24.8.24.
//


#include <Kernel/Devices/Device.h>
#include <Kernel/Devices/Storage/BlockDevice.h>
#include <Kernel/Drivers/Storage/VirtIO/BlockIODriver.h>
#include <Kernel/Drivers/VirtIO/VirtQueue.h>
#include <Utils/Types.h>

namespace Kernel {
  using Utils::as_underlying;
  using Utils::Error;
  using Utils::move;

  ErrorOr<BlockIO::Request*> BlockIODriver::block_operation(s64 request_id, u8* buffer, u64 size, u64 offset, bool is_write) {
    if(size % 512 != 0) {
      return ErrorOr<BlockIO::Request*>::create_error(Error::create_from_string("BlockIODriver: Size must be a multiple of 512 bytes."));
    }

    auto* request = new BlockIO::Request {
            .header = BlockIO::Header {
                    .type = is_write ? as_underlying(BlockIO::RequestType::Write) : as_underlying(BlockIO::RequestType::Read),
                    .reserved = 0,
                    .sector = offset / 512},
            .data = buffer,
            .status = status_value,
            .request_id = request_id};

    DebugConsole::print("BlockIODriver: Request address: ");
    DebugConsole::print_ln_number(reinterpret_cast<u64>(request), 16);

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
            .flags = as_underlying(VirtQueueDescriptorFlags::Next) | (!is_write ? as_underlying(VirtQueueDescriptorFlags::Write) : 0),
            .next = 0};

    // Move the descriptor to the VirtQueue and link next.
    virtio_device()->add_to_queue(move(descriptor));

    // The third descriptor that will tell the drive how much data to load.
    descriptor = VirtQueueDescriptor {
            .address = reinterpret_cast<u64>(&request->status),
            .length = sizeof(request->status),
            .flags = as_underlying(VirtQueueDescriptorFlags::Write),
            .next = 0};

    // Move the descriptor to the VirtQueue.
    virtio_device()->add_to_queue(move(descriptor));

    if(head_or_error.has_error()) {
      delete request;
      return ErrorOr<BlockIO::Request*>::create_error(head_or_error.get_error());
    }

    auto head = head_or_error.get_value();
    virtio_device()->add_to_available(head);
    virtio_device()->notify();

    return ErrorOr<BlockIO::Request*>::create(request);
  }

  BlockIODriver::BlockIODriver(RefPtr<BlockDevice> device) {
    m_device = move(device);
  }

  void BlockIODriver::init(RefPtr<Device> device) {
  }

  void BlockIODriver::shutdown() {
  }

  ErrorOr<void> BlockIODriver::write_async(s64 request_id, u8* buffer, u64 size, u64 offset) {
    auto result = block_operation(request_id, buffer, size, offset, true);
    if(result.has_error()) {
      return ErrorOr<void>::create_error(result.get_error());
    }
    return ErrorOr<void>::create({});
  }

  ErrorOr<void> BlockIODriver::read_async(s64 request_id, u8* buffer, u64 size, u64 offset) {
    auto result = block_operation(request_id, buffer, size, offset, false);
    if(result.has_error()) {
      return ErrorOr<void>::create_error(result.get_error());
    }
    return ErrorOr<void>::create({});
  }

  RefPtr<VirtIODevice> BlockIODriver::virtio_device() const {
    return m_device->get_underlying_device();
  }

  ErrorOr<void> BlockIODriver::write_poll(u8* buffer, u64 size, u64 offset) {
    auto result = block_operation(0, buffer, size, offset, true);
    if(result.has_error()) {
      return ErrorOr<void>::create_error(result.get_error());
    }

    auto* request = result.get_value();
    while(true) {
      if(request->status == status_value) {
        continue;
      }


      if(request->status == 0) {
        m_ignored_requests.add(request);
        return ErrorOr<void>::create({});
      }

      delete request;
      return ErrorOr<void>::create_error(Error::create_from_string("BlockIODriver: Write failed."));
    }
  }

  ErrorOr<void> BlockIODriver::read_poll(u8* buffer, u64 size, u64 offset) {
    auto result = block_operation(0, buffer, size, offset, false);
    if(result.has_error()) {
      return ErrorOr<void>::create_error(result.get_error());
    }

    auto* request = result.get_value();
    while(true) {
      if(request->status == status_value) {
        continue;
      }

      if(request->status == 0) {
        m_ignored_requests.add(request);
        return ErrorOr<void>::create({});
      }

      delete request;
      return ErrorOr<void>::create_error(Error::create_from_string("BlockIODriver: Read failed."));
    }
  }

  ErrorOr<ArrayList<RefPtr<BlockDeviceAsyncResult>>> BlockIODriver::notify() {
    ArrayList<RefPtr<BlockDeviceAsyncResult>> results;

    while(true) {
      auto descriptor_or_error = virtio_device()->get_next_used_descriptor();
      if(descriptor_or_error.has_error()) {
        break;
      }

      auto descriptor = descriptor_or_error.get_value();
      auto* request = reinterpret_cast<BlockIO::Request*>(*descriptor->address);

      auto index_of_request = m_ignored_requests.index_of(request);
      if(index_of_request != -1) {
        m_ignored_requests.remove(index_of_request);
        delete request;
        descriptor->address = 0;
        continue;
      }

      results.add(RefPtr<BlockDeviceAsyncResult>(new BlockDeviceAsyncResult(
              request->request_id,
              request->status == 0 ? BlockDeviceAsyncRequestType::Ok : BlockDeviceAsyncRequestType::Failed)));


      delete request;
      descriptor->address = 0;
    }

    return ErrorOr<ArrayList<RefPtr<BlockDeviceAsyncResult>>>::create(results);
  }

  bool VirtIODeviceDriverDescriptor::is_compatible_with(RefPtr<Device> device) {
    if(device->get_underlying_device()->get_device_type() != UnderlyingDeviceType::VirtIO) {
      return false;
    }

    if(device->get_device_type() == DeviceType::Block) {
      return true;
    }

    return false;
  }

  ErrorOr<RefPtr<DeviceDriver>> VirtIODeviceDriverDescriptor::instantiate_driver(RefPtr<Device> device) {
    if(!is_compatible_with(device)) {
      return ErrorOr<RefPtr<DeviceDriver>>::create_error(Error::create_from_string("Device is not compatible."));
    }

    if(device->get_device_type() == DeviceType::Block) {
      return ErrorOr<RefPtr<DeviceDriver>>::create(RefPtr<DeviceDriver>(new BlockIODriver(device)));
    }

    return ErrorOr<RefPtr<DeviceDriver>>::create_error(Error::create_from_string("Device is not compatible. Driver could not be instantiated."));
  }
}// namespace Kernel