//
// Created by matejbucek on 4.7.24.
//

#pragma once

#include <Kernel/VirtIO/MMIODevice.h>
#include <Utils/Arrays/ArrayList.h>
#include <Utils/Errors/ErrorOr.h>
#include <Utils/Pointers/RefCounted.h>
#include <Utils/Pointers/RefPtr.h>
#include <Utils/Types.h>
#include <Utils/Utility.h>

namespace Kernel {
  using Utils::ArrayList;
  using Utils::move;
  using Utils::Errors::Error;
  using Utils::Errors::ErrorOr;
  using Utils::Pointers::RefCounted;
  using Utils::Pointers::RefPtr;

  class UnderlyingDevice : public RefCounted<UnderlyingDevice> {
  public:
    virtual u32 get_device_id() = 0;
    virtual u32 get_vendor_id() = 0;
    virtual ~UnderlyingDevice() = default;
  };

  class VirtIODevice : public UnderlyingDevice {
  private:
    MMIODevice* m_mmio_device;

  public:
    explicit VirtIODevice(MMIODevice* mmio_device) : m_mmio_device(mmio_device) {}
    u32 get_device_id() override {
      return m_mmio_device->get_device_id();
    }
    u32 get_vendor_id() override {
      return m_mmio_device->get_vendor_id();
    }
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

  protected:
    RefPtr<UnderlyingDevice> m_underlying_device;

  public:
    explicit Device(RefPtr<UnderlyingDevice> underlying_device, DeviceType device_type = DeviceType::None) : m_device_type(device_type), m_underlying_device(move(underlying_device)) {}
    u32 get_device_id() {
      return m_underlying_device->get_device_id();
    }
    [[nodiscard]] DeviceType get_device_type() const {
      return m_device_type;
    }
    ~Device() = default;
  };

  class EntropyDevice : public Device {
  public:
    explicit EntropyDevice(RefPtr<UnderlyingDevice> underlying_device) : Device(move(underlying_device), DeviceType::Entropy) {}

  };

  class DeviceManager {
  private:
    ArrayList<RefPtr<Device>> m_devices;
    DeviceManager() = default;

  public:
    static void init();
    static DeviceManager& the();
    ErrorOr<RefPtr<Device>> try_to_load_mmio_device(uintptr_t address);
    ~DeviceManager() = default;
  };
}// namespace Kernel
