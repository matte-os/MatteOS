#pragma once

#include <Kernel/Arch/riscv64/Satp.h>
#include <Utils/Types.h>

namespace Kernel {
  struct TrapFrame {
    u64 regs[32];
    u64 fregs[32];
    uintptr_t program_counter;
    SATP satp;
    u64* trap_stack;
  };


};// namespace Kernel
