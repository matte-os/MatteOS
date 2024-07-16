//
// Created by matejbucek on 3.11.22.
//

#include <Kernel/API/Syscall.h>
#include <Kernel/Syscalls/SyscallManager.h>

namespace Kernel {
  static SyscallManager* g_syscall_manager = nullptr;

  static SyscallHandler syscall_handlers[] = {
          SYSCALL_GENERATOR(SYSCALL_HANDLER_FUN)};

  void SyscallManager::init() {
    if(g_syscall_manager == nullptr) {
      g_syscall_manager = new SyscallManager();
    }
  }

  SyscallManager& SyscallManager::the() {
    return *g_syscall_manager;
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
    auto result = handler();
    if(result.has_error()) {
      DebugConsole::println("SyscallManager: Error in syscall handler.");
    } else {
      DebugConsole::println("SyscallManager: Syscall executed successfully.");
    }
  }
}// namespace Kernel