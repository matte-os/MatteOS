#include <Kernel/Arch/riscv64/CSR.h>
#include <Kernel/Arch/riscv64/Interrupts/Interrupts.h>
#include <Kernel/Arch/riscv64/Interrupts/Plic.h>
#include <Kernel/Syscalls/SyscallManager.h>
#include <Kernel/System/InterruptManager.h>
#include <Kernel/System/Timer.h>
#include <Utils/DebugConsole.h>

namespace Kernel {
    using Utils::DebugConsole;

    size_t handle_software_interrupt(size_t sepc, size_t stval, size_t scause, size_t cpu_id, size_t sstatus) {
        DebugConsole::println("Interrupts: Handling software interrupt");
        return sepc;
    }

    size_t handle_timer_interrupt(size_t sepc, size_t stval, size_t scause, size_t cpu_id, size_t sstatus) {
        DebugConsole::println("Interrupts: Handling timer interrupt");
        Timer::the().set_timer(Timer::DEFAULT_PROCESS_TIME);
        DebugConsole::println("Interrupts: Setting timer for next interrupt");
        return sepc;
    }

    size_t handle_external_interrupt(size_t sepc, size_t stval, size_t scause, size_t cpu_id, size_t sstatus) {
        DebugConsole::println("Interrupts: Handling external interrupt");
        //auto context_id = System::the().get_current_kernel_trap_frame()->cpu_id;
        constexpr auto context_id = InterruptManager::DEFAULT_CONTEXT_ID;
        if (const auto next_interrupt = Plic::the().next(context_id); next_interrupt.has_value()) {
            const auto interrupt = next_interrupt.get_value();
            DebugConsole::print("Interrupts: Delegating interrupt to device ");
            DebugConsole::print_ln_number(interrupt, 10);
            InterruptManager::the().delegate_device_interrupt(interrupt);
            Plic::the().complete(context_id, interrupt);
            DebugConsole::print("Interrupts: Is pending: ");
            DebugConsole::print_ln_number(Plic::the().is_pending(interrupt), 10);
        }
        return sepc;
    }

    size_t handle_system_call(size_t sepc, size_t stval, size_t scause, size_t cpu_id, size_t sstatus) {
        DebugConsole::println("Interrupts: Handling system call");
        DebugConsole::print("Current time: ");
        DebugConsole::print_ln_number(Kernel::RISCV64::CSR::read<Kernel::RISCV64::CSR::Address::TIME>(), 10);
        auto* process = System::get_current_kernel_trap_frame()->current_process;
        const auto syscallId = process->get_thread()->get_trap_frame()->regs[static_cast<size_t>(
            RegisterOffset::SyscallId)];
        SyscallManager::the().handle_syscall(process, syscallId);
        return sepc + 4;
    }

    size_t unknown_interrupt(size_t sepc, size_t stval, size_t scause, size_t cpu_id, size_t sstatus) {
        DebugConsole::println("Interrupts: No handler for this interrupt");
        return sepc;
    }

    [[noreturn]] void kernel_panic(size_t sepc, size_t scause) {
        DebugConsole::println("Kernel panic!");
        DebugConsole::print("sepc: ");
        DebugConsole::print_ln_number(sepc, 16);
        DebugConsole::print("scause: ");
        DebugConsole::print_ln_number(scause, 16);
        DebugConsole::print("Interrupt: ");
        DebugConsole::println(interrupt_to_string(static_cast<Interrupts>(scause)));
        asm volatile("wfi");
    }
} // namespace Kernel

extern "C" size_t handle_interrupt(size_t sepc, size_t stval, size_t scause, size_t cpu_id, size_t sstatus,
                                   bool kernel_flag) {
    if (kernel_flag) {
        DebugConsole::println("Interrupts: Kernel flag is set");
        auto cause = static_cast<Kernel::Interrupts>(scause);
        if (cause == Kernel::Interrupts::InstructionPageFault || cause == Kernel::Interrupts::LoadPageFault || cause ==
            Kernel::Interrupts::StorePageFault || cause == Kernel::Interrupts::IllegalInstruction) {
            Kernel::kernel_panic(sepc, scause);
        }

        if (cause == Kernel::Interrupts::ExternalInterrupt) {
            return Kernel::handle_external_interrupt(sepc, stval, scause, cpu_id, sstatus);
        }
        return sepc;
    }

    switch (static_cast<Kernel::Interrupts>(scause)) {
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
