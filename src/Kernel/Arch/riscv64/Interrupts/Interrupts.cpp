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

  size_t handle_software_interrupt(size_t sepc, size_t stval, size_t scause, size_t cpu_id, size_t sstatus) {
    return sepc;
  }

  size_t handle_timer_interrupt(size_t sepc, size_t stval, size_t scause, size_t cpu_id, size_t sstatus) {
    if(auto* current_thread = ProcessManager::the().get_current_thread()) {
      current_thread->get_trap_frame()->program_counter = sepc;
    }

    const auto thread = Scheduler::the().schedule();

    // Write process SATP to the SSCRATCH register
    RISCV64::CSR::write<RISCV64::CSR::Address::SSCRATCH>(*reinterpret_cast<u64*>(reinterpret_cast<u64>(&thread->get_trap_frame()->satp)));

    const auto program_counter = thread->get_trap_frame()->program_counter;

    Timer::the().set_timer(Timer::DEFAULT_PROCESS_TIME);
    return program_counter;
  }

  size_t handle_external_interrupt(size_t sepc, size_t stval, size_t scause, size_t cpu_id, size_t sstatus) {
    //auto context_id = System::the().get_current_kernel_trap_frame()->cpu_id;
    constexpr auto context_id = InterruptManager::DEFAULT_CONTEXT_ID;
    if(const auto next_interrupt = Plic::the().next(context_id); next_interrupt.has_value()) {
      const auto interrupt = next_interrupt.value();
      InterruptManager::the().delegate_device_interrupt(interrupt);
      Plic::the().complete(context_id, interrupt);
    }
    return sepc;
  }

  size_t handle_system_call(size_t sepc, size_t stval, size_t scause, size_t cpu_id, size_t sstatus) {
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
      current_thread->get_trap_frame()->program_counter = sepc;
      const auto new_process = Scheduler::the().schedule();

      // Write process SATP to the SSCRATCH register
      RISCV64::CSR::write<RISCV64::CSR::Address::SSCRATCH>(*reinterpret_cast<u64*>(reinterpret_cast<u64>(&new_process->get_trap_frame()->satp)));

      return new_process->get_trap_frame()->program_counter;
    }
    return sepc + 4;
  }

  size_t unknown_interrupt(size_t sepc, size_t stval, size_t scause, size_t cpu_id, size_t sstatus) {
    dbgln("Interrupts: No handler for interrupt {}, on address: {16}", scause, sepc);
    return sepc;
  }

  [[noreturn]] void kernel_panic(size_t sepc, size_t scause) {
    Logger::the().switch_to_sbi();
    dbgln("Interrupts: Kernel panic, cause: {} on address: {}.", scause, sepc);
    SBI::sbi_hart_stop();
    asm volatile("wfi");
  }
}// namespace Kernel

extern "C" size_t handle_interrupt(size_t sepc, size_t stval, size_t scause, size_t cpu_id, size_t sstatus,
                                   bool kernel_flag) {
  if(kernel_flag) {
    auto cause = static_cast<Kernel::Interrupts>(scause);
    if(cause == Kernel::Interrupts::InstructionPageFault || cause == Kernel::Interrupts::LoadPageFault || cause == Kernel::Interrupts::StorePageFault || cause == Kernel::Interrupts::IllegalInstruction) {
      Kernel::kernel_panic(sepc, scause);
    }

    if(cause == Kernel::Interrupts::ExternalInterrupt) {
      return Kernel::handle_external_interrupt(sepc, stval, scause, cpu_id, sstatus);
    }
    return sepc;
  }

  s64 allocated_bytes_on_int_start = Kernel::KernelMemoryAllocator::the().get_statistics().used_size;
  s64 allocated_by_logger_start = Kernel::Logger::the().get_buffer_size();
  size_t ret_sepc;

  switch(static_cast<Kernel::Interrupts>(scause)) {
    case Kernel::Interrupts::SoftwareInterrupt: {
      ret_sepc = Kernel::handle_software_interrupt(sepc, stval, scause, cpu_id, sstatus);
    } break;
    case Kernel::Interrupts::TimerInterrupt: {
      ret_sepc =  Kernel::handle_timer_interrupt(sepc, stval, scause, cpu_id, sstatus);
    }break;
    case Kernel::Interrupts::ExternalInterrupt: {
      ret_sepc =  Kernel::handle_external_interrupt(sepc, stval, scause, cpu_id, sstatus);
    }break;
    case Kernel::Interrupts::SystemCall: {
      ret_sepc =  Kernel::handle_system_call(sepc, stval, scause, cpu_id, sstatus);
    }break;
    default: {
      ret_sepc =  Kernel::unknown_interrupt(sepc, stval, scause, cpu_id, sstatus);
    }break;
  }

  s64 allocated_bytes_on_int_end = Kernel::KernelMemoryAllocator::the().get_statistics().used_size;
  s64 allocated_by_logger_end = Kernel::Logger::the().get_buffer_size();
  s64 total = allocated_bytes_on_int_end - allocated_bytes_on_int_start - (allocated_by_logger_end - allocated_by_logger_start);
  if(total > 300) {
    dbgln("Interrupts: Warning, memory allocation during interrupt (int: {}) handling detected! ({} bytes)", scause, total);
  }

  return ret_sepc;
}
