#pragma once

#include <Kernel/Arch/Interrupts.h>
#include <Utils/Arrays/Array.h>
#include <Utils/Errors/ErrorOr.h>
#include <Utils/Function.h>
#include <Utils/Types.h>

namespace Kernel {
  using Utils::Array;
  using Utils::ErrorOr;
  using Utils::Function;

  class InterruptManager {
  public:
    static void init();
    static InterruptManager& the();
  };

}// namespace Kernel
