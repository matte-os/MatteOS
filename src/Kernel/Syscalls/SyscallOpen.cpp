#include <Kernel/API/Syscall.h>
#include <Kernel/FileSystem/VirtualFileSystem.h>
#include <Kernel/Process/Process.h>

using Utils::ErrorOr;

namespace Kernel {
  ErrorOr<uintptr_t, SysError> Process::handle_open(SyscallString path, u64 flags) {
    auto vfs = VirtualFileSystem::the();
    auto file = vfs.open(m_credentials, "", flags);
    if(file.has_error()) {
    }

    return ErrorOr<uintptr_t, SysError>::create(0);
  }

  ErrorOr<uintptr_t, SysError> Process::handle_close(u64 fd) {
    DebugConsole::print("Closing file descriptor: ");
    DebugConsole::print_ln_number(fd, 10);
    return ErrorOr<uintptr_t, SysError>::create(12345);
  }
}// namespace Kernel