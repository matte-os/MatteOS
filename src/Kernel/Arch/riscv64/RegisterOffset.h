#pragma once

#include <Utils/Types.h>

namespace Kernel {
  namespace Arch::RISCV64 {
    enum class RegisterOffset : size_t {
      ZERO = 0,
      RA = 1,
      SP = 2,
      GP = 3,
      TP = 4,
      T0 = 5,
      T1 = 6,
      T2 = 7,
      S0 = 8,
      S1 = 9,
      A0 = 10,
      A1 = 11,
      A2 = 12,
      A3 = 13,
      A4 = 14,
      A5 = 15,
      A6 = 16,
      A7 = 17,
      S2 = 18,
      S3 = 19,
      S4 = 20,
      S5 = 21,
      S6 = 22,
      S7 = 23,
      S8 = 24,
      S9 = 25,
      S10 = 26,
      S11 = 27,
      T3 = 28,
      T4 = 29,
      T5 = 30,
      T6 = 31
    };
  }

  enum class RegisterOffset : size_t {
    GP0 = static_cast<size_t>(Arch::RISCV64::RegisterOffset::A0),
    GP1 = static_cast<size_t>(Arch::RISCV64::RegisterOffset::A1),
    GP2 = static_cast<size_t>(Arch::RISCV64::RegisterOffset::A2),
    GP3 = static_cast<size_t>(Arch::RISCV64::RegisterOffset::A3),
    GP4 = static_cast<size_t>(Arch::RISCV64::RegisterOffset::A4),
    GP5 = static_cast<size_t>(Arch::RISCV64::RegisterOffset::A5),
    GP6 = static_cast<size_t>(Arch::RISCV64::RegisterOffset::A6),
    GP7 = static_cast<size_t>(Arch::RISCV64::RegisterOffset::A7),
    SP = static_cast<size_t>(Arch::RISCV64::RegisterOffset::SP),
    RA = static_cast<size_t>(Arch::RISCV64::RegisterOffset::RA),
    SyscallId = static_cast<size_t>(Arch::RISCV64::RegisterOffset::A7)
  };
}