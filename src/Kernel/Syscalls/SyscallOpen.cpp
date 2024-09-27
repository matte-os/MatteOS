#include <Kernel/API/Syscall.h>
#include <Kernel/FileSystem/File.h>
#include <Kernel/FileSystem/VirtualFileSystem.h>
#include <Kernel/Process/Process.h>

using Utils::ErrorOr;

namespace Kernel {
  ErrorOr<uintptr_t, SysError> Process::handle_open(SyscallString path, u64 flags) {
    auto& vfs = VirtualFileSystem::the();
    auto file_or_error = vfs.open(m_credentials, "", flags);
    if(file_or_error.has_error()) {
      DebugConsole::print("Failed to open file: ");
      DebugConsole::println(file_or_error.get_error().get_message().value());
    }

    //auto open_or_error = ErrorOr<size_t>::create(0);
    auto open_or_error = m_fd_table.open(RefPtr<File>(new File(file_or_error.get_value(), flags)));
    if(open_or_error.has_error()) {
      DebugConsole::print("Failed to open file: ");
      DebugConsole::println(open_or_error.get_error().get_message().value());
    }

    DebugConsole::println("Opened file successfully");
    return ErrorOr<uintptr_t, SysError>::create(open_or_error.get_value());
  }

  ErrorOr<uintptr_t, SysError> Process::handle_close(u64 fd) {
    auto descriptor = m_fd_table[fd];
    if(!descriptor) {
      return ErrorOr<uintptr_t, SysError>::create_error(SysError::Error);
    }

    m_fd_table.close(fd);

    DebugConsole::print("Closing file descriptor: ");
    DebugConsole::print_ln_number(fd, 10);
    return ErrorOr<uintptr_t, SysError>::create(0);
  }
}// namespace Kernel