/**
 * @file Device.h
 * @author Matěj Bucek
 */
#pragma once

#include <Kernel/Drivers/DeviceDriver.h>
#include <Kernel/Drivers/VirtIO/MMIODevice.h>
#include <Kernel/Drivers/VirtIO/VirtQueue.h>
#include <Utils/Arrays/Array.h>
#include <Utils/Arrays/ArrayList.h>
#include <Utils/Errors/ErrorOr.h>
#include <Utils/Function.h>
#include <Utils/Pair.h>
#include <Utils/Pointers/RefCounted.h>
#include <Utils/Pointers/RefPtr.h>
#include <Utils/Strings/String.h>
#include <Utils/Traits.h>
#include <Utils/Types.h>

namespace Kernel {
  using Utils::Array;
  using Utils::ArrayList;
  using Utils::Error;
  using Utils::ErrorOr;
  using Utils::Function;
  using Utils::move;
  using Utils::Pair;
  using Utils::RefCounted;
  using Utils::RefPtr;
  using Utils::String;

  enum class UnderlyingDeviceType {
    VirtIO,
    SBIConsole,
    Universal,
  };

  class UnderlyingDevice : public RefCounted<UnderlyingDevice> {
  private:
    UnderlyingDeviceType m_device_type;

  public:
    explicit UnderlyingDevice(UnderlyingDeviceType device_type) : m_device_type(device_type) {}

    [[nodiscard]] UnderlyingDeviceType get_device_type() const {
      return m_device_type;
    }

    virtual u32 get_device_id() = 0;
    virtual u32 get_vendor_id() = 0;
    virtual void reset() = 0;

    template<typename T>
    T* as() {
      return static_cast<T*>(this);
    }

    virtual ~UnderlyingDevice() = default;
  };

  class VirtIODevice final : public UnderlyingDevice {
  private:
    MMIODevice* m_mmio_device;
    u32 m_features;
    VirtQueueArray m_virt_queues;
    RefPtr<Array<u64>> m_queue_indexes;
    RefPtr<Array<u64>> m_queue_acks;
    u32 m_selected_queue = 0;

  public:
    explicit VirtIODevice(MMIODevice* mmio_device) : UnderlyingDevice(UnderlyingDeviceType::VirtIO), m_mmio_device(mmio_device), m_virt_queues(), m_queue_indexes(), m_queue_acks() {}

    u32 get_device_id() override {
      return m_mmio_device->get_device_id();
    }

    u32 get_vendor_id() override {
      return m_mmio_device->get_vendor_id();
    }

    virtual ErrorOr<void> init(u32 features, u64 number_of_virt_queues, Function<void, VirtQueue*, u64> init_virt_queue);
    void reset() override;

    u64 get_next_queue_index(u32 selected_queue);
    ErrorOr<u32> add_to_queue(VirtQueueDescriptor&& descriptor);
    ErrorOr<void> add_to_available(u32 descriptor_index);
    ErrorOr<void> notify();
    ErrorOr<VirtQueueDescriptor*> get_next_used_descriptor();
  };

  class SBIConsoleDevice : public UnderlyingDevice {
  public:
    explicit SBIConsoleDevice() : UnderlyingDevice(UnderlyingDeviceType::SBIConsole) {}

    u32 get_device_id() override {
      return 0;
    }

    u32 get_vendor_id() override {
      return 0;
    }

    void reset() override;
  };

  class UniversalDevice : public UnderlyingDevice {
  private:
    u32 m_device_id;
    u32 m_vendor_id;
    String m_device_tree_path;
    String m_device_type;
    size_t m_address;

  public:
    UniversalDevice(u32 device_id, u32 vendor_id, String device_tree_path, String device_type, size_t address) : UnderlyingDevice(UnderlyingDeviceType::Universal), m_device_id(device_id), m_vendor_id(vendor_id), m_device_tree_path(move(device_tree_path)), m_device_type(move(device_type)), m_address(address) {}

    u32 get_device_id() override {
      return m_device_id;
    }

    u32 get_vendor_id() override {
      return m_vendor_id;
    }

    String get_device_tree_path() {
      return m_device_tree_path;
    }

    String get_device_type() {
      return m_device_type;
    }

    size_t get_address() {
      return m_address;
    }

    void reset() override;
  };

  enum class DeviceType : size_t {
    None = 0,
    Network = 1,
    Block = 2,
    Console = 3,
    Entropy = 4,
    Gpu,
    Input,
    Memory,
  };

  class Device : public RefCounted<Device> {
    DeviceType m_device_type;
    ArrayList<u64> m_interrupts;
    bool m_needs_driver;

  protected:
    RefPtr<DeviceDriver> m_driver;
    RefPtr<UnderlyingDevice> m_underlying_device;

    ErrorOr<void> create_dev_inode();

  public:
    explicit Device(RefPtr<UnderlyingDevice> underlying_device,
                    ArrayList<u64>&& interrupts,
                    DeviceType device_type = DeviceType::None,
                    bool needs_driver = false) : m_device_type(device_type), m_interrupts(move(interrupts)), m_underlying_device(move(underlying_device)), m_needs_driver(needs_driver) {}

    u32 get_device_id() {
      return m_underlying_device->get_device_id();
    }

    [[nodiscard]] DeviceType get_device_type() const {
      return m_device_type;
    }

    RefPtr<UnderlyingDevice> get_underlying_device() const {
      return m_underlying_device;
    }

    [[nodiscard]] const ArrayList<u64>& get_interrupts() const {
      return m_interrupts;
    }

    [[nodiscard]] bool needs_driver() const {
      return m_needs_driver;
    }

    void set_driver(RefPtr<DeviceDriver> driver) {
      m_driver = move(driver);
    }

    [[nodiscard]] bool has_driver() const {
      return m_driver;
    }

    template<typename T>
    T* as() {
      return static_cast<T*>(this);
    }

    [[nodiscard]]
    bool handles_interrupt(u64);
    virtual ErrorOr<void> handle_interrupt(u64 interrupt_id);
    virtual ErrorOr<void> init();
    virtual ~Device() = default;
  };
}// namespace Kernel
