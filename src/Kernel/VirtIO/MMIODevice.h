//
// Created by matejbucek on 4.7.24.
//

#pragma once

#include <Utils/Types.h>
#include <Kernel/VirtIO/MMIODeviceRegisterLayout.h>

namespace Kernel {
  class MMIODevice {
  private:
    MMIODevice() = default;
    template <typename T>
    T get_register_value(MMIODeviceRegisterLayout offset) {
      return *reinterpret_cast<T*>(reinterpret_cast<char*>(this) + static_cast<size_t>(offset));
    }
  public:
    u32 get_magic_value() {
      return get_register_value<u32>(MMIODeviceRegisterLayout::MagicValue);
    }
    u32 get_version() {
      return get_register_value<u32>(MMIODeviceRegisterLayout::Version);
    }
    u32 get_device_id() {
      return get_register_value<u32>(MMIODeviceRegisterLayout::DeviceId);
    }
    u32 get_vendor_id() {
      return get_register_value<u32>(MMIODeviceRegisterLayout::VendorId);
    }
    ~MMIODevice() = default;
  };
}// namespace Kernel
