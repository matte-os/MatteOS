#pragma once

#include <Kernel/System/TrapFrame.h>

namespace Kernel {
  class Process;
  struct KernelTrapFrame : public TrapFrame {
    u64 cpu_id;
    Process* current_process;
  };
}