/**
 * @file SyscallManager.cpp
 * @author Matěj Bucek
 *
 * This code was inspired by SerenityOS's implementation.
 */
#include <Kernel/API/Syscall.h>
#include <Kernel/Process/Process.h>
#include <Kernel/Syscalls/SyscallManager.h>
#include <Utils/Assertions.h>

namespace Kernel {
  using SysResult = Utils::ErrorOr<uintptr_t, SysError>;
  using SyscallHandler = auto (Process::*)(uintptr_t arg0, uintptr_t arg1, uintptr_t arg2,
                                           uintptr_t arg3, uintptr_t arg4, uintptr_t arg5,
                                           uintptr_t arg6) -> SysResult;


  static SyscallManager* s_syscall_manager = nullptr;

#define SYSCALL_HANDLER(name) reinterpret_cast<SyscallHandler>((&Process::handle_##name)),
  static SyscallHandler syscall_handlers[] = {
          SYSCALL_GENERATOR(SYSCALL_HANDLER)};
#undef SYSCALL_HANDLER

  void SyscallManager::init() {
    if(s_syscall_manager == nullptr) { s_syscall_manager = new SyscallManager(); }
  }

  SyscallManager& SyscallManager::the() {
    runtime_assert(s_syscall_manager, "SyscallManager not initialized.");
    return *s_syscall_manager;
  }

  ErrorOr<void, SysError> SyscallManager::handle_syscall(Process* process, u64 syscall_id) {
    if(syscall_id >= (u64) Syscalls::__Count) { return {}; }

    auto handler = syscall_handlers[syscall_id];
    DebugConsole::print("SyscallManager: Process ");
    DebugConsole::print_number((u64) process, 16);
    DebugConsole::print(" called syscall ");
    DebugConsole::println(syscall_names[syscall_id]);

    auto trap_frame = process->get_thread()->get_trap_frame();

    auto arg0 = trap_frame->get_register<u64>(RegisterOffset::GP0);
    auto arg1 = trap_frame->get_register<u64>(RegisterOffset::GP1);
    auto arg2 = trap_frame->get_register<u64>(RegisterOffset::GP2);
    auto arg3 = trap_frame->get_register<u64>(RegisterOffset::GP3);
    auto arg4 = trap_frame->get_register<u64>(RegisterOffset::GP4);
    auto arg5 = trap_frame->get_register<u64>(RegisterOffset::GP5);
    auto arg6 = trap_frame->get_register<u64>(RegisterOffset::GP6);

    DebugConsole::print("SyscallManager: Arguments: ");
    DebugConsole::print_number(arg0, 16);
    DebugConsole::print(", ");
    DebugConsole::print_number(arg1, 16);
    DebugConsole::print(", ");
    DebugConsole::print_number(arg2, 16);
    DebugConsole::print(", ");
    DebugConsole::print_number(arg3, 16);
    DebugConsole::print(", ");
    DebugConsole::print_number(arg4, 16);
    DebugConsole::print(", ");
    DebugConsole::print_number(arg5, 16);
    DebugConsole::print(", ");
    DebugConsole::print_number(arg6, 16);
    DebugConsole::print(", ");
    DebugConsole::print_ln_number(syscall_id, 16);


    auto result = (process->*handler)(arg0, arg1, arg2, arg3, arg4, arg5, arg6);
    if(result.has_error()) {
      if(result.get_error() == SysError::Blocked) {
        return SysError::Blocked;
      }
      DebugConsole::println("SyscallManager: Error in syscall handler.");
      trap_frame->set_register<u64>(RegisterOffset::GP0, static_cast<u64>(-1));
    } else {
      DebugConsole::println("SyscallManager: Syscall executed successfully.");
      trap_frame->set_register<u64>(RegisterOffset::GP0, result.get_value());
    }

    return {};
  }
}// namespace Kernel