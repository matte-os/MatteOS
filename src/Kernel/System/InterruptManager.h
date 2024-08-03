#pragma once

#include <Kernel/Arch/Interrupts.h>
#include <Kernel/System/DeviceManager.h>
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
  private:
    HashMap<InterruptId, RefPtr<Device>> m_device_interrupts;

  public:
    static void init();
    static InterruptManager& the();
    void register_device_interrupt(InterruptId interrupt_id, const RefPtr<Device>& device);
    void unregister_device_interrupt(InterruptId interrupt_id);
    void handle_interrupt(InterruptId interrupt_id);
  };

}// namespace Kernel
