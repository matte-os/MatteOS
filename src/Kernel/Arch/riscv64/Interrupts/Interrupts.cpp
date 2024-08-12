#include <Kernel/Arch/riscv64/CSR.h>
#include <Kernel/Arch/riscv64/Interrupts/Interrupts.h>
#include <Kernel/Syscalls/SyscallManager.h>
#include <Kernel/System/InterruptManager.h>
#include <Utils/DebugConsole.h>

namespace Kernel {
  using Utils::DebugConsole;

  size_t handle_software_interrupt(size_t sepc, size_t stval, size_t scause, size_t cpu_id, size_t sstatus) {
    DebugConsole::println("Interrupts: Handling software interrupt");
    return sepc;
  }

  size_t handle_timer_interrupt(size_t sepc, size_t stval, size_t scause, size_t cpu_id, size_t sstatus) {
    DebugConsole::println("Interrupts: Handling timer interrupt");
    return sepc;
  }

  size_t handle_external_interrupt(size_t sepc, size_t stval, size_t scause, size_t cpu_id, size_t sstatus) {
    DebugConsole::println("Interrupts: Handling external interrupt");
    // FIXME: Interrupt id should not be scause, it should be the actual interrupt id
    // from PLIC.
    InterruptManager::the().delegate_device_interrupt(scause);
    return sepc;
  }

  size_t handle_system_call(size_t sepc, size_t stval, size_t scause, size_t cpu_id, size_t sstatus) {
    DebugConsole::println("Interrupts: Handling system call");
    DebugConsole::print("Current time: ");
    DebugConsole::print_ln_number(Kernel::RISCV64::CSR::read<Kernel::RISCV64::CSR::Address::TIME>(), 10);
    auto* process = System::get_current_kernel_trap_frame()->current_process;
    auto syscallId = process->get_thread()->get_trap_frame()->regs[static_cast<size_t>(Kernel::RegisterOffset::SyscallId)];
    SyscallManager::the().handle_syscall(process, syscallId);
    return sepc + 4;
  }

  size_t unknown_interrupt(size_t sepc, size_t stval, size_t scause, size_t cpu_id, size_t sstatus) {
    DebugConsole::println("Interrupts: No handler for this interrupt");
    return sepc;
  }

}// namespace Kernel

extern "C" size_t handle_interrupt(size_t sepc, size_t stval, size_t scause, size_t cpu_id, size_t sstatus) {
  switch(static_cast<Kernel::Interrupts>(scause)) {
    case Kernel::Interrupts::SoftwareInterrupt: {
      return Kernel::handle_software_interrupt(sepc, stval, scause, cpu_id, sstatus);
    }
    case Kernel::Interrupts::TimerInterrupt: {
      return Kernel::handle_timer_interrupt(sepc, stval, scause, cpu_id, sstatus);
    }
    case Kernel::Interrupts::ExternalInterrupt: {
      return Kernel::handle_external_interrupt(sepc, stval, scause, cpu_id, sstatus);
    }
    case Kernel::Interrupts::SystemCall: {
      return Kernel::handle_system_call(sepc, stval, scause, cpu_id, sstatus);
    }
    default: {
      return Kernel::unknown_interrupt(sepc, stval, scause, cpu_id, sstatus);
    }
  }
}