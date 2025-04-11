#pragma once

#include <Kernel/System/TrapFrame.h>

namespace Kernel {
  class Process;
  struct KernelTrapFrame : TrapFrame {
    u64 cpu_id;
    size_t current_thread_id;
    size_t current_process_id;
  };
}