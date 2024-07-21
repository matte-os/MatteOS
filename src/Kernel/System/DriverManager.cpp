//
// Created by matejbucek on 6.7.24.
//

#include <Kernel/System/DriverManager.h>
#include <Utils/Assertions.h>
#include <Utils/DebugConsole.h>
using Utils::DebugConsole;

namespace Kernel {
  DriverManager* s_driver_manager = nullptr;

  void DriverManager::init() {
    if(s_driver_manager == nullptr) {
      s_driver_manager = new DriverManager;
    } else {
      DebugConsole::println("DriverManager: Already initialised.");
    }
  }

  DriverManager& Kernel::DriverManager::the() {
    runtime_assert(s_driver_manager, "DriverManager: Not initialised.");
    return *s_driver_manager;
  }
}// namespace Kernel
