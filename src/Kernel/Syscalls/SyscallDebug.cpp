#include <Kernel/API/Syscall.h>
#include <Kernel/Logger.h>
#include <Kernel/Process/Process.h>
#include <Kernel/Process/Userspace.h>

namespace Kernel {
  ErrorOr<uintptr_t, SysError> Process::handle_dbgln(Userspace<char*> message) {
    auto error_or_message_in_kernel = message.get(m_page_table);
    if(error_or_message_in_kernel.has_error()) {
      return ErrorOr<uintptr_t, SysError>::create_error(SysError::Error);
    }

    DebugConsole::println(error_or_message_in_kernel.get_value());
    return ErrorOr<uintptr_t, SysError>::create(0);
  }

  ErrorOr<uintptr_t, SysError> Process::handle_dmesg() {
    Logger::the().print_logfile();
    return {0};
  }
  ErrorOr<uintptr_t, SysError> Process::handle_stats() {
    dbglog_direct("\\{\"cpus\": [\\{\"id\": 0, \"time\": {}}]}\n", Kernel::RISCV64::CSR::read<RISCV64::CSR::Address::TIME>());
    return {0};
  }
}// namespace Kernel