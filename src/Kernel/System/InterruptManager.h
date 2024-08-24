#pragma once

#include <Kernel/Arch/Interrupts.h>
#include <Kernel/Devices/DeviceManager.h>
#include <Utils/Arrays/Array.h>
#include <Utils/Errors/ErrorOr.h>
#include <Utils/Function.h>
#include <Utils/Maps/HashMap.h>
#include <Utils/Types.h>

namespace Kernel {
  using Utils::Array;
  using Utils::ErrorOr;
  using Utils::Function;
  using Utils::HashMap;

  using InterruptId = u64;

  class InterruptManager {
  public:
    static void init();
    static InterruptManager& the();
    ErrorOr<void> handle_interrupt(InterruptId interrupt_id);
    ErrorOr<void> delegate_device_interrupt(InterruptId interrupt_id);
  };

}// namespace Kernel
