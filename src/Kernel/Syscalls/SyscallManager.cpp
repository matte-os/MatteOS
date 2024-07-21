//
// Created by matejbucek on 3.11.22.
//

#include <Kernel/API/Syscall.h>
#include <Kernel/Process/Process.h>
#include <Kernel/Syscalls/SyscallManager.h>
#include <Utils/Assertions.h>

namespace Kernel {
  using SysResult = Utils::ErrorOr<uintptr_t, SysError>;
  using SyscallHandler = auto (Process::*)()-> SysResult;


  static SyscallManager* s_syscall_manager = nullptr;

#define SYSCALL_HANDLER(name) reinterpret_cast<SyscallHandler>((&Process::handle_##name)),
  static SyscallHandler syscall_handlers[] = {
          SYSCALL_GENERATOR(SYSCALL_HANDLER)
  };
#undef SYSCALL_HANDLER

  void SyscallManager::init() {
    if(s_syscall_manager == nullptr) {
      s_syscall_manager = new SyscallManager();
    }
  }

  SyscallManager& SyscallManager::the() {
    runtime_assert(s_syscall_manager, "SyscallManager not initialized.");
    return *s_syscall_manager;
  }

  void SyscallManager::handle_syscall(Process* process, u64 syscall_id) {
    if(syscall_id >= (u64) Syscalls::__Count) {
      return;
    }

    auto handler = syscall_handlers[syscall_id];
    DebugConsole::print("SyscallManager: Process ");
    DebugConsole::print_number((u64) process, 16);
    DebugConsole::print(" called syscall ");
    DebugConsole::print_ln_number(syscall_id, 10);
    auto result = (process->*(handler))();
    if(result.has_error()) {
      DebugConsole::println("SyscallManager: Error in syscall handler.");
    } else {
      DebugConsole::println("SyscallManager: Syscall executed successfully.");
    }
  }
}// namespace Kernel