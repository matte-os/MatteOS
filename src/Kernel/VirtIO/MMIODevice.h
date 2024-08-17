//
// Created by matejbucek on 4.7.24.
//

#pragma once

#include <Kernel/VirtIO/MMIODeviceRegisterLayout.h>
#include <Utils/Types.h>

namespace Kernel {
  class MMIODevice {
  private:
    MMIODevice() = default;

    template<typename T>
    T get_register_value(MMIODeviceRegisterLayout offset) {
      return *reinterpret_cast<T*>(reinterpret_cast<char*>(this) + static_cast<size_t>(offset));
    }

    void set_register_value(MMIODeviceRegisterLayout offset, u32 value) {
      *reinterpret_cast<u32*>(reinterpret_cast<char*>(this) + static_cast<size_t>(offset)) = value;
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

    u32 get_status() {
      return get_register_value<u32>(MMIODeviceRegisterLayout::Status);
    }

    void set_status(u32 value) {
      set_register_value(MMIODeviceRegisterLayout::Status, value);
    }

    u32 get_host_features() {
      return get_register_value<u32>(MMIODeviceRegisterLayout::HostFeatures);
    }

    void set_host_features(u32 value) {
      set_register_value(MMIODeviceRegisterLayout::HostFeatures, value);
    }

    u32 get_guest_features() {
      return get_register_value<u32>(MMIODeviceRegisterLayout::GuestFeatures);
    }

    void set_guest_features(u32 value) {
      set_register_value(MMIODeviceRegisterLayout::GuestFeatures, value);
    }

    u32 get_queue_num_max() {
      return get_register_value<u32>(MMIODeviceRegisterLayout::QueueNumMax);
    }

    void set_queue_num(u32 value) {
      set_register_value(MMIODeviceRegisterLayout::QueueNum, value);
    }

    void set_guest_page_size(u32 value) {
      set_register_value(MMIODeviceRegisterLayout::GuestPageSize, value);
    }

    void set_queue_sel(u32 value) {
      set_register_value(MMIODeviceRegisterLayout::QueueSel, value);
    }

    u32 get_queue_sel() {
      return get_register_value<u32>(MMIODeviceRegisterLayout::QueueSel);
    }

    u32 get_queue_pfn() {
      return get_register_value<u32>(MMIODeviceRegisterLayout::QueuePfn);
    }

    void set_queue_pfn(u32 value) {
      set_register_value(MMIODeviceRegisterLayout::QueuePfn, value);
    }

    void set_queue_notify(u32 value) {
      set_register_value(MMIODeviceRegisterLayout::QueueNotify, value);
    }

    ~MMIODevice() = default;
  };
}// namespace Kernel
