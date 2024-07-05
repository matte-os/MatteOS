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

  class Device : public RefCounted<Device> {
  private:
    RefPtr<UnderlyingDevice> m_underlying_device;

  public:
    explicit Device(RefPtr<UnderlyingDevice> underlying_device) : m_underlying_device(move(underlying_device)) {}
    ~Device() = default;
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
