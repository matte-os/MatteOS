/**
* @file InterruptManager.h
 * @author Matěj Bucek
 */
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
  using ContextId = u64;

  class InterruptManager {
  public:
    static const ContextId DEFAULT_CONTEXT_ID = 1;

    static void init();
    static InterruptManager& the();
    ErrorOr<void> handle_interrupt(InterruptId interrupt_id);
    ErrorOr<void> delegate_device_interrupt(InterruptId interrupt_id);
    void enable_interrupt(ContextId context_id, InterruptId interrupt_id);
    void disable_interrupt(ContextId context_id, InterruptId interrupt_id);
    void enable_device_interrupts();
    void set_threshold(u64 threshold);
  };

}// namespace Kernel
