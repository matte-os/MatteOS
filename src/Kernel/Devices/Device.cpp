/**
 * @file Device.cpp
 * @author Matěj Bucek
 */
#include <Kernel/Devices/Device.h>
#include <Kernel/Drivers/Storage/VirtIO/BlockIODriver.h>
#include <Kernel/Devices/DeviceManager.h>

namespace Kernel {
  using Utils::as_underlying;

  ErrorOr<void> Device::init() {
    return ErrorOr<void>::create_error(Error::create_from_string("Device slot is empty."));
  }

  ErrorOr<void> Device::handle_interrupt(u64 interrupt_id) {
    DebugConsole::println("Device: Default device interrupt handler called");
    return ErrorOr<void>::create({});
  }

  bool Device::handles_interrupt(u64 interrupt_id) {
    auto interrupt_or_error = m_interrupts.find_first_match([interrupt_id](auto interrupt) -> bool {
      return interrupt_id == interrupt;
    });

    return interrupt_or_error.has_value();
  }

  // https://docs.oasis-open.org/virtio/virtio/v1.2/csd01/virtio-v1.2-csd01.html#x1-1070001
  // 3.1.1 Driver Requirements: Device Initialization
  ErrorOr<void> VirtIODevice::init(u32 features, u64 number_of_virt_queues, Function<void, VirtQueue*, u64> init_virt_queue) {
    m_queue_indexes = Array<u64>(number_of_virt_queues);
    m_queue_indexes.fill(0);
    m_queue_acks = Array<u64>(number_of_virt_queues);
    m_queue_acks.fill(0);

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
    m_mmio_device->set_guest_page_size(PAGE_SIZE);

    // Allocate memory for the VirtQueues and initialize them.
    m_virt_queues = VirtQueueArray(number_of_virt_queues);
    for(u64 i = 0; i < number_of_virt_queues; i++) {
      auto queue_address = MemoryManager::the().zalloc(number_of_pages);
      // Store the address of the VirtQueue for quick access.
      m_virt_queues[i] = reinterpret_cast<VirtQueue*>(queue_address);
      init_virt_queue(m_virt_queues[i], i);
    }

    // Set the address of the VirtQueue in the device.
    auto queue_address_value = reinterpret_cast<size_t>(m_virt_queues[0]);
    m_mmio_device->set_queue_pfn(static_cast<u32>(queue_address_value) / PAGE_SIZE);

    // 8. Set the DRIVER_OK status bit. At this point the device is “live”.
    status |= static_cast<u32>(StatusField::DriverOK);
    m_mmio_device->set_status(status);
    return ErrorOr<void>::create({});
  }

  void VirtIODevice::reset() {
    m_mmio_device->set_status(0);
  }

  ErrorOr<u32> VirtIODevice::add_to_queue(VirtQueueDescriptor&& descriptor) {
    auto selected_queue = m_selected_queue;
    auto queue = m_virt_queues[selected_queue];

    auto available_index = get_next_queue_index(selected_queue);
    queue->descriptors[available_index] = move(descriptor);
    if(*descriptor.flags & as_underlying(VirtQueueDescriptorFlags::Next)) {
      queue->descriptors[available_index].next = (available_index + 1) % VIRTIO_RING_SIZE;
    }

    return ErrorOr<u32>::create(available_index);
  }

  ErrorOr<VirtQueueDescriptor*> VirtIODevice::get_next_used_descriptor() {
    auto selected_queue = m_selected_queue;
    auto queue = m_virt_queues[selected_queue];

    if(m_queue_acks[selected_queue] == *queue->used.index) {
      return ErrorOr<VirtQueueDescriptor*>::create_error(Error::create_from_string("No used descriptor available."));
    }

    auto ack = m_queue_acks[selected_queue];
    auto element = queue->used.ring[ack % VIRTIO_RING_SIZE];
    auto* descriptor = &queue->descriptors[element.id];

    ack = (ack + 1) % VIRTIO_RING_SIZE;
    m_queue_acks[selected_queue] = ack;

    return ErrorOr<VirtQueueDescriptor*>::create(descriptor);
  }

  u64 VirtIODevice::get_next_queue_index(u32 selected_queue) {
    auto index = m_queue_indexes[selected_queue];
    m_queue_indexes[selected_queue] = (index + 1) % VIRTIO_RING_SIZE;
    return index;
  }

  ErrorOr<void> VirtIODevice::add_to_available(u32 descriptor_index) {
    auto selected_queue = m_selected_queue;
    auto queue = m_virt_queues[selected_queue];

    queue->available.ring[*queue->available.index % VIRTIO_RING_SIZE] = descriptor_index;
    queue->available.index++;

    return ErrorOr<void>::create({});
  }

  ErrorOr<void> VirtIODevice::notify() {
    m_mmio_device->set_queue_notify(m_selected_queue);
    return ErrorOr<void>::create({});
  }

  void SBIConsoleDevice::reset() {
  }

  void UniversalDevice::reset() {
  }

  ErrorOr<void> Device::create_dev_inode() {
      return DeviceManager::the().create_dev_inode(RefPtr(this));
  }
}// namespace Kernel