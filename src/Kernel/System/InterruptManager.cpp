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
}// namespace Kernel