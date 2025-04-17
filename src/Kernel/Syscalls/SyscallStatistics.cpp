/**
 * @file SyscallStatistics.cpp
 * @author Matěj Bucek
 */
#include <Kernel/Syscalls/SyscallStatistics.h>

namespace Kernel {
  void SyscallStatistics::increment(const Syscalls syscall) {
    m_syscall_count[as_underlying(syscall)]++;
  }

  void SyscallStatistics::increment(const u64 syscall_id) {
    m_syscall_count[syscall_id]++;

  }

  void SyscallStatistics::for_each(Callable<void, Syscalls, u64> callable) {
    for(size_t i = 0; i < m_syscall_count.size(); i++) {
      callable(static_cast<Syscalls>(i), m_syscall_count[i]);
    }
  }
}// namespace Kernel