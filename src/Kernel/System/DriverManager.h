//
// Created by matejbucek on 6.7.24.
//

#pragma once

#include <Kernel/Forwards.h>
#include <Utils/Arrays/ArrayList.h>
#include <Utils/Errors/ErrorOr.h>
#include <Utils/Pointers/RefCounted.h>

namespace Kernel {

  using Utils::ArrayList;
  using Utils::ErrorOr;

  class Driver : public RefCounted<Driver> {
  public:
    virtual void init(RefPtr<Device> device) = 0;
    virtual void shutdown() = 0;

    template<typename T>
    T* as() {
      return static_cast<T*>(this);
    }
  };

  class BlockIODriver : public Driver {
    RefPtr<BlockDevice> m_device;

  public:
    explicit BlockIODriver(RefPtr<BlockDevice> device);
    ErrorOr<void> write(u8* buffer, u64 size, u64 offset);
    ErrorOr<void> read(u8* buffer, u64 size, u64 offset);
    void init(RefPtr<Device> device) override;
    void shutdown() override;

  private:
    ErrorOr<void> block_operation(u8* buffer, u64 size, u64 offset, bool is_write);
    RefPtr<VirtIODevice> virtio_device() const;
  };

  class DriverDescriptor : public RefCounted<DriverDescriptor> {
  public:
    virtual bool is_compatible_with(RefPtr<Device> device) = 0;
    virtual ErrorOr<RefPtr<Driver>> instantiate_driver(RefPtr<Device> device) = 0;
  };

  class VirtIODriverDescriptor : public DriverDescriptor {
  public:
    bool is_compatible_with(RefPtr<Device> device) override;
    ErrorOr<RefPtr<Driver>> instantiate_driver(RefPtr<Device> device) override;
  };

  class DriverManager {
  private:
    ArrayList<RefPtr<DriverDescriptor>> m_driver_descriptors;

  public:
    static DriverManager& the();
    static void init();
    ArrayList<RefPtr<DriverDescriptor>> find_compatible_drivers(RefPtr<Device> device);

  private:
    DriverManager() = default;
    void register_default_drivers();
    void register_driver(RefPtr<DriverDescriptor> driver_descriptor);
  };
}// namespace Kernel