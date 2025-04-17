/**
 * @file SyscallProcess.cpp
 * @author Matěj Bucek
 */
#include <Kernel/API/Syscall.h>
#include <Kernel/Process/Process.h>
#include <Kernel/Process/ProcessManager.h>
#include <Kernel/Process/Userspace.h>

using Utils::ErrorOr;

namespace Kernel {
  ErrorOr<uintptr_t, SysError> Process::handle_exit(int exit_code) {
    m_state = ProcessState::Dead;
    return {exit_code};
  }
}