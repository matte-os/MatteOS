/**
 * @file SyscallManager.cpp
 * @author Matěj Bucek
 *
 * This code was inspired by SerenityOS's implementation.
 */
#include <Kernel/API/Syscall.h>
#include <Kernel/Logger.h>
#include <Kernel/Process/Process.h>
#include <Kernel/Process/ProcessManager.h>
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
    dbgln("SyscallManager: Process {} called syscall {}", process->get_pid(), syscall_id);
    m_statistics.increment(syscall_id);

    auto current_thread = ProcessManager::the().get_current_thread();

    auto trap_frame = current_thread->get_trap_frame();

    auto arg0 = trap_frame->get_register<u64>(RegisterOffset::GP0);
    auto arg1 = trap_frame->get_register<u64>(RegisterOffset::GP1);
    auto arg2 = trap_frame->get_register<u64>(RegisterOffset::GP2);
    auto arg3 = trap_frame->get_register<u64>(RegisterOffset::GP3);
    auto arg4 = trap_frame->get_register<u64>(RegisterOffset::GP4);
    auto arg5 = trap_frame->get_register<u64>(RegisterOffset::GP5);
    auto arg6 = trap_frame->get_register<u64>(RegisterOffset::GP6);


    dbgln("SyscallManager: Arguments: {16}, {16}, {16}, {16}, {16}, {16}, {16}", arg0, arg1, arg2, arg3, arg4, arg5, arg6, syscall_id);

    auto result = (process->*handler)(arg0, arg1, arg2, arg3, arg4, arg5, arg6);
    if(result.has_error()) {
      if(result.get_error() == SysError::Blocked) {
        return SysError::Blocked;
      }
      dbgln("SyscallManager: Error in syscall handler {}!", syscall_id);
      trap_frame->set_register<u64>(RegisterOffset::GP0, static_cast<u64>(-1));
    } else {
      dbgln("SyscallManager: Successfully handled syscall {}", syscall_id);
      trap_frame->set_register<u64>(RegisterOffset::GP0, result.get_value());
    }

    return {};
  }
}// namespace Kernel