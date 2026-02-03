/**
 * @file Scheduler.h
 * @author Matěj Bucek
 */
#pragma once
#include "Process.h"
#include <Kernel/System/TrapFrame.h>

namespace Kernel {
  class Scheduler {
  public:
    static void init();
    static Scheduler& the();
    Thread* schedule();
    [[noreturn]] void start_scheduling();

  private:
    Scheduler() = default;
    void prepare_for_switch(Thread* target_thread);
  };
}// namespace Kernel
