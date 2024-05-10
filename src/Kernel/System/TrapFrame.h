#pragma once

#include "Kernel/Satp.h"
#include "Utils/Types.hh"

namespace Kernel {
  struct TrapFrame {
    u64 regs[32];
    u64 fregs[32];
    uintptr_t program_counter;
    SATP satp;
    u64* trap_stack;
  };

  struct KernelTrapFrame : public TrapFrame {
    u64 cpu_id;
  };
};// namespace Kernel
