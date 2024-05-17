//
// Created by matejbucek on 3.11.22.
//

#include <Kernel/System/SyscallManager.h>

namespace Kernel::System {
  static SyscallManager* g_syscall_manager = nullptr;
  void SyscallManager::init() {
    if(g_syscall_manager == nullptr) {
      g_syscall_manager = new SyscallManager();
    }
  }

  SyscallManager& SyscallManager::the() {
    return *g_syscall_manager;
  }
  void SyscallManager::handle_syscall(Process::Process* process, u64 syscall_id) {
  }
}// namespace Kernel::System