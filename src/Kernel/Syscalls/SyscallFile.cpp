#include <Kernel/API/Syscall.h>
#include <Kernel/FileSystem/File.h>
#include <Kernel/FileSystem/VirtualFileSystem.h>
#include <Kernel/Process/Process.h>
#include <Kernel/Process/ProcessManager.h>
#include <Kernel/Process/Userspace.h>

using Utils::ErrorOr;

namespace Kernel {
  ErrorOr<uintptr_t, SysError> Process::handle_open(Userspace<char*> path, u64 flags) {
    auto& vfs = VirtualFileSystem::the();

    // Translate the path from userspace to kernel space
    auto error_or_path_in_kernel = path.get(m_page_table);
    if(error_or_path_in_kernel.has_error()) {
      return SysError::Error;
    }

    // Copies the path from userspace to kernel space
    auto path_in_kernel = String(error_or_path_in_kernel.get_value());

    auto file_or_error = vfs.open(m_credentials, path_in_kernel, flags);
    if(file_or_error.has_error()) {
      DebugConsole::print("Failed to open file: ");
      DebugConsole::println(file_or_error.get_error().get_message().value());
      return SysError::Error;
    }

    //auto open_or_error = ErrorOr<size_t>::create(0);
    auto open_or_error = m_fd_table.open(RefPtr<File>(new File(file_or_error.get_value(), flags)));
    if(open_or_error.has_error()) {
      DebugConsole::print("Failed to open file: ");
      DebugConsole::println(open_or_error.get_error().get_message().value());
      return SysError::Error;
    }

    DebugConsole::println("Opened file successfully");
    return open_or_error.get_value();
  }

  ErrorOr<uintptr_t, SysError> Process::handle_close(u64 fd) {
    auto descriptor = m_fd_table[fd];
    if(!descriptor) {
      return SysError::Error;
    }

    m_fd_table.close(fd);

    DebugConsole::print("Closing file descriptor: ");
    DebugConsole::print_ln_number(fd, 10);
    return ErrorOr<uintptr_t, SysError>::create(0);
  }

  ErrorOr<uintptr_t, SysError> Process::handle_read(int file_descriptor, Userspace<u8*> buffer, size_t size) {
    DebugConsole::print("Reading from file descriptor: ");
    DebugConsole::print_ln_number(file_descriptor, 10);
    DebugConsole::print("Reading from buffer: ");
    DebugConsole::print_ln_number(static_cast<u64>(buffer.virtual_address()), 16);
    DebugConsole::print("Buffer size: ");
    DebugConsole::print_ln_number(size, 10);
    auto file = m_fd_table[file_descriptor];
    auto error_or_buffer = buffer.get(m_page_table);

    if(error_or_buffer.has_error()) {
      return SysError::Error;
    }

    auto error_or_request = file->read(error_or_buffer.get_value(), size);

    if(error_or_request.has_error()) {
      return SysError::Error;
    }

    auto request = error_or_request.get_value();

    if(request.is_blocked()) {
      request.assign_pid(m_pid);
      ProcessManager::the().block_process(this);
      return SysError::Blocked;
    }

    return request.release_value();
  }
}// namespace Kernel