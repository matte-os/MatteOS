#pragma once

#include <Kernel/Devices/Device.h>

namespace Kernel {

  class EntropyDevice : public Device {
  public:
    explicit EntropyDevice(RefPtr<UnderlyingDevice> underlying_device, ArrayList<u64>&& interrupts) : Device(move(underlying_device), move(interrupts), DeviceType::Entropy) {}

    ErrorOr<void> init() override;
  };

}// namespace Kernel
