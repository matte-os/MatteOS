//
// Created by matejbucek on 4.7.24.
//

#pragma once

#include <Kernel/VirtIO/MMIODevice.h>
#include <Kernel/VirtIO/VirtQueue.h>
#include <Utils/Arrays/ArrayList.h>
#include <Utils/Errors/ErrorOr.h>
#include <Utils/Function.h>
#include <Utils/Pointers/RefCounted.h>
#include <Utils/Pointers/RefPtr.h>
#include <Utils/Strings/String.h>
#include <Utils/Traits.h>
#include <Utils/Types.h>

namespace Kernel {
  using Utils::ArrayList;
  using Utils::move;
  using Utils::Error;
  using Utils::ErrorOr;
  using Utils::RefCounted;
  using Utils::RefPtr;
  using Utils::Function;
  using Utils::String;

  enum class UnderlyingDeviceType {
    VirtIO,
    SBIConsole,
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
    virtual ErrorOr<void> init(u32 features, Function<void, VirtQueue*> init_virt_queue) = 0;
    virtual void reset() = 0;
    virtual ~UnderlyingDevice() = default;
  };

  class VirtIODevice : public UnderlyingDevice {
  private:
    MMIODevice* m_mmio_device;
    // FIXME: Add support for multiple VirtQueues
    // We should have a member variable that specifies the number of VirtQueues
    // to be initialized.
    VirtQueue* m_virt_queue;

  public:
    explicit VirtIODevice(MMIODevice* mmio_device) : UnderlyingDevice(UnderlyingDeviceType::VirtIO), m_mmio_device(mmio_device), m_virt_queue(nullptr) {}
    u32 get_device_id() override {
      return m_mmio_device->get_device_id();
    }
    u32 get_vendor_id() override {
      return m_mmio_device->get_vendor_id();
    }
    ErrorOr<void> init(u32 features, Function<void, VirtQueue*> init_virt_queue) override;
    void reset() override;
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
    ErrorOr<void> init(u32 features, Function<void, VirtQueue*> init_virt_queue = nullptr) override;
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
  private:
    DeviceType m_device_type;
    ArrayList<u64> m_interrupts;

  protected:
    RefPtr<UnderlyingDevice> m_underlying_device;

  public:
    explicit Device(RefPtr<UnderlyingDevice> underlying_device, ArrayList<u64>&& interrupts, DeviceType device_type = DeviceType::None) : m_device_type(device_type), m_interrupts(move(interrupts)), m_underlying_device(move(underlying_device)) {}
    u32 get_device_id() {
      return m_underlying_device->get_device_id();
    }
    [[nodiscard]] DeviceType get_device_type() const {
      return m_device_type;
    }
    virtual ErrorOr<void> handle_interrupt(u64 interrupt_id);
    virtual ErrorOr<void> init();
    virtual ~Device() = default;
  };

  class EntropyDevice : public Device {
  public:
    explicit EntropyDevice(RefPtr<UnderlyingDevice> underlying_device, ArrayList<u64>&& interrupts) : Device(move(underlying_device), move(interrupts), DeviceType::Entropy) {}
    ErrorOr<void> init() override;
  };

  class BlockDevice : public Device {
  public:
    explicit BlockDevice(RefPtr<UnderlyingDevice> underlying_device, ArrayList<u64>&& interrupts) : Device(move(underlying_device), move(interrupts), DeviceType::Block) {}
    ErrorOr<void> init() override;
  };

  class ConsoleDevice : public Device {
  public:
    explicit ConsoleDevice(RefPtr<UnderlyingDevice> underlying_device, ArrayList<u64>&& interrupts) : Device(move(underlying_device), move(interrupts), DeviceType::Console) {}
    ErrorOr<void> init() override;
    ErrorOr<void> write(String message);
    ErrorOr<String> read();
    ErrorOr<void> handle_interrupt(u64 interrupt_id) override;
  };

  class DeviceManager {
  private:
    ArrayList<RefPtr<Device>> m_devices;
    DeviceManager() = default;

  public:
    static void init();
    static DeviceManager& the();
    ErrorOr<RefPtr<Device>> try_to_load_mmio_device(uintptr_t address, ArrayList<u64>&& interrupts);
    ErrorOr<void> add_device(const RefPtr<Device>& device);
    ~DeviceManager() = default;
  };
}// namespace Kernel
