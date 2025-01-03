#include <Kernel/Arch/PLIC.h>
#include <Kernel/System/InterruptManager.h>
#include <Utils/Assertions.h>
#include <Utils/DebugConsole.h>
#include <Utils/Errors/ErrorOr.h>

namespace Kernel {
  static InterruptManager* s_interruptManager = nullptr;

  using Utils::DebugConsole;
  using Utils::Error;

  void InterruptManager::init() {
    if(s_interruptManager == nullptr) {
      s_interruptManager = new InterruptManager();
      Plic::init();
    } else {
      DebugConsole::println("InterruptManager: Already initialised.");
    }
  }

  InterruptManager& InterruptManager::the() {
    runtime_assert(s_interruptManager, "InterruptManager: Not initialized.");
    return *s_interruptManager;
  }

  ErrorOr<void> InterruptManager::handle_interrupt(InterruptId interrupt_id) {
    return ErrorOr<void>::create_error(Error::create_from_string("InterruptManager: Not implemented."));
  }

  ErrorOr<void> InterruptManager::delegate_device_interrupt(InterruptId interrupt_id) {
    return DeviceManager::the().delegate_device_interrupt(interrupt_id);
  }

  void InterruptManager::enable_interrupt(ContextId context_id, InterruptId interrupt_id) {
    Plic::the().enable(context_id, interrupt_id);
  }

  void InterruptManager::disable_interrupt(ContextId context_id, InterruptId interrupt_id) {
    Plic::the().disable(context_id, interrupt_id);
  }

  void InterruptManager::enable_device_interrupts() {
    DebugConsole::println("InterruptManager: Enabling device interrupts.");
    DebugConsole::print("InterruptManager: Number of devices: ");
    DebugConsole::print_ln_number(DeviceManager::the().get_devices().size(), 10);
    for(auto& device: DeviceManager::the().get_devices()) {
      DebugConsole::println("InterruptManager: Enabling interrupts for device ");
      //Number of interrupts should be based on the number of harts
      DebugConsole::print("InterruptManager: Number of interrupts: ");
      DebugConsole::print_ln_number(device->get_interrupts().size(), 10);
      device->get_interrupts().for_each([&](auto interrupt_id) {
        DebugConsole::println("InterruptManager: Enabling interrupt ");
        //FIXME: The context_id should be based on the number of harts
        Plic::the().enable(DEFAULT_CONTEXT_ID, interrupt_id);
        Plic::the().set_priority(interrupt_id, 1);
      });
    }
  }

  void InterruptManager::set_threshold(u64 threshold) {
    Plic::the().set_threshold(DEFAULT_CONTEXT_ID, threshold);
  }
}// namespace Kernel