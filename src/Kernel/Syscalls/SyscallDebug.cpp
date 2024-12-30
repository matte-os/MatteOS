#include <Kernel/API/Syscall.h>
#include <Kernel/Process/Process.h>
#include <Kernel/Process/Userspace.h>

namespace Kernel {
  ErrorOr<uintptr_t, SysError> Process::handle_dbgln(Userspace<char*> message) {
    DebugConsole::print("Message from process: ");
    auto error_or_message_in_kernel = message.get(m_page_table);
    if(error_or_message_in_kernel.has_error()) {
      return ErrorOr<uintptr_t, SysError>::create_error(SysError::Error);
    }

    DebugConsole::println(error_or_message_in_kernel.get_value());
    return ErrorOr<uintptr_t, SysError>::create(0);
  }
}// namespace Kernel