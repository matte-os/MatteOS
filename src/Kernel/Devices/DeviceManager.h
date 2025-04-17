/**
 * @file DeviceManager.h
 * @author Matěj Bucek
 */
#pragma once

#include <Kernel/Devices/Device.h>
#include <Kernel/Drivers/VirtIO/MMIODevice.h>
#include <Kernel/Drivers/VirtIO/VirtQueue.h>
#include <Utils/Arrays/Array.h>
#include <Utils/Arrays/ArrayList.h>
#include <Utils/Errors/ErrorOr.h>
#include <Utils/Function.h>
#include <Utils/Maps/HashMap.h>
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
  using Utils::RefCounted;
  using Utils::RefPtr;
  using Utils::String;

  class DeviceManager {
    ArrayList<RefPtr<Device>> m_devices;
    RefPtr<RamFileSystem> m_devfs;
    DeviceManager();

  protected:
    friend class Device;
    ErrorOr<void> create_dev_inode(RefPtr<Device> device);

  public:
    static void init();
    static DeviceManager& the();
    ErrorOr<RefPtr<Device>> try_to_load_mmio_device(uintptr_t address, ArrayList<u64>&& interrupts);
    ErrorOr<void> add_device(const RefPtr<Device>& device);
    ErrorOr<void> delegate_device_interrupt(u64 interrupt_id);
    void load_drivers();

    ArrayList<RefPtr<Device>>& get_devices() {
      return m_devices;
    }

    ArrayList<RefPtr<Device>> get_devices_of_type(DeviceType type);
    ~DeviceManager() = default;
  };
}// namespace Kernel
