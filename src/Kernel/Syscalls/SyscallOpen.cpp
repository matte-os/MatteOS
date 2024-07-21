#include <Kernel/API/Syscall.h>
#include <Kernel/FileSystem/VirtualFileSystem.h>
#include <Kernel/Process/Process.h>

using Utils::ErrorOr;

namespace Kernel {
  ErrorOr<uintptr_t, SysError> Process::handle_open(u64 syscall_id) {
    auto result = VirtualFileSystem::the();
    return ErrorOr<uintptr_t, SysError>::create(0);
  }

  ErrorOr<uintptr_t, SysError> Process::handle_close() {
    return ErrorOr<uintptr_t, SysError>::create(0);
  }
}// namespace Kernel