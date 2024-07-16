//
// Created by matejbucek on 21.10.22.
//

#pragma once
#include "Process.h"
#include <Kernel/System/TrapFrame.h>

namespace Kernel {
  class Scheduler {
  public:
    static void init();
    static Scheduler& the();
    Process* schedule();
    [[noreturn]] void start_scheduling();

  private:
    Scheduler() = default;
  };
}// namespace Kernel
