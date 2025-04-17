/**
 * @file SyscallDebug.cpp
 * @author Matěj Bucek
 */
#include "SyscallManager.h"


#include <Kernel/API/Syscall.h>
#include <Kernel/Arch/riscv64/CSR.h>
#include <Kernel/FileSystem/VirtualFileSystem.h>
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
    auto statistics = SyscallManager::the().get_statistics();
    String syscalls;
    statistics.for_each([&syscalls](const Syscalls syscall, const u64 count) {
      syscalls += format("\"{}\": {}, ", syscall_names[as_underlying(syscall)], count);
    });

    dbglog_direct("\\{\"cpus\": [\\{\"id\": 0, \"time\": {}, \"syscalls\": \\{ {} }}}\n", Kernel::RISCV64::CSR::read<RISCV64::CSR::Address::TIME>(), syscalls);
    return {0};
  }

  ErrorOr<uintptr_t, SysError> Process::handle_ls() {
    auto error_or_list = VirtualFileSystem::the().list_directory("/");

    if(error_or_list.has_error()) {
      return SysError::Error;
    }

    error_or_list.get_value().for_each([](const auto &entry) {
      dbglog_direct("{}\n", entry.get_name());
    });

    return {0};
  }

  ErrorOr<uintptr_t, SysError> Process::handle_mem() {
    auto memory_statistics = MemoryManager::the().get_statistics();
    auto kernel_memory_statistics = KernelMemoryAllocator::the().get_statistics();

    auto memory = format("\\{\"total_pages\": {}, \"free_pages\": {}, \"used_pages\": {}}", memory_statistics.total_pages, memory_statistics.free_pages, memory_statistics.used_pages);
    auto kernel_memory = format("\\{\"total_bytes\": {}, \"free_bytes\": {}, \"used_bytes\": {}}", kernel_memory_statistics.total_size, kernel_memory_statistics.free_size, kernel_memory_statistics.used_size);

    dbglog_direct("\\{\"memory\": {}, \"kernel_memory_allocator\": {}, \"logfile\": {}}\n", memory, kernel_memory, Logger::the().get_buffer_size());
    return {0};
  }
}// namespace Kernel