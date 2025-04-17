/**
 * @file SyscallManager.h
 * @brief Contains the SyscallManager class and all the handlers for syscalls.
 *
 * @author Matěj Bucek (matejbucek)
 */

#pragma once


#include <Kernel/Process/Process.h>
#include <Kernel/Syscalls/SyscallStatistics.h>
#include <Utils/Function.h>
#include <Utils/Strings/String.h>
#include <Utils/Types.h>

using Kernel::Process;
using Utils::Function;
using Utils::String;

namespace Kernel {
  /**
   * @brief SyscallManager is a singleton class that manages syscalls.
   */
  class SyscallManager {
    SyscallStatistics m_statistics;

  public:
    static void init();
    static SyscallManager& the();
    ErrorOr<void, SysError> handle_syscall(Process* process, u64 syscall_id);
    const SyscallStatistics& get_statistics() const { return m_statistics; }
  };
}// namespace Kernel
