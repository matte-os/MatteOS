/**
 * @file Interrupts.cpp
 * @author Matěj Bucek
 */

#include <Kernel/Arch/riscv64/CSR.h>
#include <Kernel/Arch/riscv64/Interrupts/Interrupts.h>
#include <Kernel/Arch/riscv64/Interrupts/Plic.h>
#include <Kernel/Logger.h>
#include <Kernel/Process/ProcessManager.h>
#include <Kernel/Process/Scheduler.h>
#include <Kernel/SBI/SBI.h>
#include <Kernel/Syscalls/SyscallManager.h>
#include <Kernel/System/InterruptManager.h>
#include <Kernel/System/Timer.h>
#include <Utils/DebugConsole.h>

namespace Kernel {
  using Utils::DebugConsole;

  static bool is_kernel_trap(TrapFrame* tf) {
    return (tf->sstatus & (1ULL << 8)) != 0;
  }

  TrapFrame* handle_software_interrupt(TrapFrame* tf) {
    return tf;
  }

  TrapFrame* handle_timer_interrupt(TrapFrame* tf) {
    auto* next_thread = Scheduler::the().schedule();

    Timer::the().set_timer(Timer::DEFAULT_PROCESS_TIME);

    return next_thread->get_trap_frame();
  }

  TrapFrame* handle_external_interrupt(TrapFrame* tf) {
    //auto context_id = System::the().get_current_kernel_trap_frame()->cpu_id;
    constexpr auto context_id = InterruptManager::DEFAULT_CONTEXT_ID;
    if(const auto next_interrupt = Plic::the().next(context_id); next_interrupt.has_value()) {
      const auto interrupt = next_interrupt.value();
      InterruptManager::the().delegate_device_interrupt(interrupt);
      Plic::the().complete(context_id, interrupt);
    }
    return tf;
  }

  TrapFrame* handle_system_call(TrapFrame* tf) {
    auto* process = ProcessManager::the().get_current_process();
    auto* current_thread = ProcessManager::the().get_current_thread();
    const auto syscallId = current_thread->get_trap_frame()->regs[static_cast<size_t>(
            RegisterOffset::SyscallId)];

    s64 log_size_before = Logger::the().get_buffer_size();
    s64 mem_usage_before = KernelMemoryAllocator::the().get_statistics().used_size;
    auto error_or_ok = SyscallManager::the().handle_syscall(process, syscallId);
    s64 mem_usage_after = KernelMemoryAllocator::the().get_statistics().used_size;
    s64 log_size_after = Logger::the().get_buffer_size();
    dbgln("Memory usage change during syscall: {} bytes", mem_usage_after - mem_usage_before - (log_size_after - log_size_before));

    if(error_or_ok.has_error() && error_or_ok.get_error() == SysError::Blocked) {
      // Reschedule a different process
      const auto new_process = Scheduler::the().schedule();
      return new_process->get_trap_frame();
    }
    tf->sepc += 4;
    return tf;
  }

  TrapFrame* unknown_interrupt(TrapFrame* tf) {
    dbgln("Interrupts: No handler for interrupt {}, on address: {16}", tf->scause, tf->sepc);
    return tf;
  }

  [[noreturn]] void kernel_panic(TrapFrame* tf) {
    Logger::the().switch_to_sbi();
    dbgln("Interrupts: Kernel panic, cause: {} on address: {}.", tf->scause, tf->sepc);
    SBI::sbi_hart_stop();
    asm volatile("wfi");
    while(true);
  }
}// namespace Kernel

extern "C" Kernel::TrapFrame* handle_interrupt(Kernel::TrapFrame* tf) {
  if(Kernel::is_kernel_trap(tf)) {
    auto cause = static_cast<Kernel::Interrupts>(tf->scause);
    if(cause == Kernel::Interrupts::InstructionPageFault || cause == Kernel::Interrupts::LoadPageFault || cause == Kernel::Interrupts::StorePageFault || cause == Kernel::Interrupts::IllegalInstruction) {
      Kernel::kernel_panic(tf);
    }

    if(cause == Kernel::Interrupts::ExternalInterrupt) {
      return Kernel::handle_external_interrupt(tf);
    }
    return tf;
  }

  s64 allocated_bytes_on_int_start = Kernel::KernelMemoryAllocator::the().get_statistics().used_size;
  s64 allocated_by_logger_start = Kernel::Logger::the().get_buffer_size();

  Kernel::TrapFrame* next_tf = tf;

  switch(static_cast<Kernel::Interrupts>(tf->scause)) {
    case Kernel::Interrupts::SoftwareInterrupt: {
      next_tf = Kernel::handle_software_interrupt(tf);
    } break;
    case Kernel::Interrupts::TimerInterrupt: {
      next_tf = Kernel::handle_timer_interrupt(tf);
    } break;
    case Kernel::Interrupts::ExternalInterrupt: {
      next_tf = Kernel::handle_external_interrupt(tf);
    } break;
    case Kernel::Interrupts::SystemCall: {
      next_tf = Kernel::handle_system_call(tf);
    } break;
    default: {
      next_tf = Kernel::unknown_interrupt(tf);
    } break;
  }

  s64 allocated_bytes_on_int_end = Kernel::KernelMemoryAllocator::the().get_statistics().used_size;
  s64 allocated_by_logger_end = Kernel::Logger::the().get_buffer_size();
  s64 total = allocated_bytes_on_int_end - allocated_bytes_on_int_start - (allocated_by_logger_end - allocated_by_logger_start);
  if(total > 300) {
    dbgln("Interrupts: Warning, memory allocation during interrupt (int: {}) handling detected! ({} bytes)", tf->scause, total);
  }

  return next_tf;
}
