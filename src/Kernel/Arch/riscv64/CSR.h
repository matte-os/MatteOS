/**
 * @file CSR.h
 * @author Matěj Bucek
 *
 * This code was inspired by SerenityOS's implementation.
 */
#pragma once

#include <Utils/Types.h>

namespace Kernel::RISCV64::CSR {
  enum class Address : u16 {
    // Supervisor Trap Setup
    SSTATUS = 0x100,
    SIE = 0x104,
    STVEC = 0x105,

    // Supervisor Protection and Translation
    SATP = 0x180,

    // Unprivileged Counters/Timers
    CYCLE = 0xc00,
    TIME = 0xc01,

    SSCRATCH = 0x140,
  };

  template<Address addr>
  u64 read() {
    u64 value;
    asm volatile("csrr %0, %1" : "=r"(value) : "i"(static_cast<u16>(addr)));
    return value;
  }

  template<Address addr>
  void write(u64 value) {
    asm volatile("csrw %0, %1" : : "i"(static_cast<u16>(addr)), "r"(value));
  }
}// namespace Kernel::RISCV64::CSR