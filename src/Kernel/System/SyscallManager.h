/**
 * @file SyscallManager.h
 * @brief Contains the SyscallManager class and all the handlers for syscalls.
 *
 * @author Matěj Bucek (matejbucek)
 */

#pragma once

#include <Kernel/Process/Process.h>
#include <Utils/Types.h>

using Kernel::Process::Process;

namespace Kernel::System {
  /**
   * @brief SyscallManager is a singleton class that manages syscalls.
   */
  class SyscallManager {
  public:
    static void init();
    static SyscallManager& the();
    void handle_syscall(Process::Process* process, u64 syscall_id);
  };
}// namespace Kernel::System
