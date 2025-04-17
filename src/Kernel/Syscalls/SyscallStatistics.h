/**
 * @file SyscallStatistics.h
 * @author Matěj Bucek
 */
#pragma once

#include <Kernel/API/Syscall.h>
#include <Utils/Arrays/Array.h>
#include <Utils/Callable.h>

namespace Kernel {
  using Utils::Array;
  using Utils::as_underlying;
  using Utils::Callable;

  class SyscallStatistics {
    Array<u64> m_syscall_count;

  public:
    SyscallStatistics() : m_syscall_count(as_underlying(Syscalls::__Count)) {}

    void increment(Syscalls syscall);
    void increment(u64 syscall_id);
    void for_each(Callable<void, Syscalls, u64> callable);
  };

}// namespace Kernel
