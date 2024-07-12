//
// Created by matejbucek on 6.7.24.
//

#include <Kernel/System/DriverManager.h>
#include <Utils/DebugConsole.h>
using Utils::DebugConsole;

namespace Kernel {
  DriverManager* g_driver_manager = nullptr;

  void DriverManager::init() {
    if(g_driver_manager == nullptr) {
      g_driver_manager = new DriverManager;
    } else {
      DebugConsole::println("DriverManager: Already initialised.");
    }
  }

  DriverManager& Kernel::DriverManager::the() {
    return *g_driver_manager;
  }
}// namespace Kernel
