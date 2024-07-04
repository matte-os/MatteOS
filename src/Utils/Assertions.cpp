#include <Kernel/CPU.h>
#include <Utils/Assertions.h>
#include <Utils/DebugConsole.h>

using Kernel::CPU;

namespace Utils {
  void ASSERT(bool condition) {
    if(!condition) {
      DebugConsole::println("Assetion");
      CPU::halt();
    }
  }
};// namespace Utils