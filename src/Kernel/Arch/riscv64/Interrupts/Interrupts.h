#pragma once

#include <Utils/Types.h>
namespace Kernel {
  namespace Arch::RISCV64::SCAUSE {
    enum class Async : size_t {
      SoftwareInterrupt = 0x8000000000000001,
      TimerInterrupt = 0x8000000000000005,
      ExternalInterrupt = 0x8000000000000009,
      CounterOverflowInterrupt = 0x800000000000000d,
    };

    enum class Sync : size_t {
      InstructionAddressMisaligned = 0,
      InstructionAccessFault = 1,
      IllegalInstruction = 2,
      Breakpoint = 3,
      LoadAddressMisaligned = 4,
      LoadAccessFault = 5,
      StoreAMOAddressMisaligned = 6,
      StoreAMOAccessFault = 7,
      EnvironmentCallFromUMode = 8,
      EnvironmentCallFromSMode = 9,
      InstructionPageFault = 12,
      LoadPageFault = 13,
      StoreAMOPageFault = 15,
      SoftwareCheck = 18,
      HardwareError = 19,
    };
  }// namespace Arch::RISCV64::SCAUSE

#define INTERRUPT_GENERATOR(F)                                                        \
  F(SoftwareInterrupt, Arch::RISCV64::SCAUSE::Async::SoftwareInterrupt)               \
  F(TimerInterrupt, Arch::RISCV64::SCAUSE::Async::TimerInterrupt)                     \
  F(ExternalInterrupt, Arch::RISCV64::SCAUSE::Async::ExternalInterrupt)               \
  F(CounterOverflowInterrupt, Arch::RISCV64::SCAUSE::Async::CounterOverflowInterrupt) \
  F(SystemCall, Arch::RISCV64::SCAUSE::Sync::EnvironmentCallFromUMode)

  enum class Interrupts : size_t {
#undef __ENUMERATE_INTERRUPT
#define __ENUMERATE_INTERRUPT(name, value) name = static_cast<size_t>(value),
    INTERRUPT_GENERATOR(__ENUMERATE_INTERRUPT)
#undef __ENUMERATE_INTERRUPT
  };

  constexpr size_t number_of_interrupts() {
#undef __ENUMERATE_INTERRUPT
#define __ENUMERATE_INTERRUPT(name, value) +1
    return 0 INTERRUPT_GENERATOR(__ENUMERATE_INTERRUPT);
#undef __ENUMERATE_INTERRUPT
  }
}// namespace Kernel

extern "C" size_t handle_interrupt(size_t sepc, size_t stval, size_t scause, size_t cpu_id, size_t sstatus);
