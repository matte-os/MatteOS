/**
 * @file Process.h
 * @author Matěj Bucek
 *
 * This file was inspired by SerenityOS.
 */
#pragma once

#include "Userspace.h"
#include <Kernel/API/Syscall.h>
#include <Kernel/Arch/riscv64/CPU.h>
#include <Kernel/FileSystem/FileDescriptorTable.h>
#include <Kernel/Memory/MemoryManager.h>
#include <Kernel/Memory/MemoryRegion.h>
#include <Kernel/Memory/PageTable.h>
#include <Kernel/Process/Thread.h>
#include <Kernel/Security/Credentials.h>
#include <Utils/Arrays/Array.h>
#include <Utils/Arrays/ArrayList.h>
#include <Utils/Pointers/RefPtr.h>
#include <Utils/Types.h>

using Kernel::MemoryManager;
using Kernel::MemoryRegion;
using Kernel::PageTable;
using Utils::Array;
using Utils::RefPtr;

namespace Kernel {
  enum class ProcessState : u8 {
    Running,
    Sleeping,
    Blocked,
    Dead
  };

  class Process {
    friend class ProcessManager;

  protected:
    size_t m_pid;
    size_t m_gid;
    PageTable* m_page_table;
    ProcessState m_state;
    Credentials m_credentials;
    FileDescriptorTable m_fd_table;
    ArrayList<Thread*> m_threads;

  public:
    explicit Process(size_t pid,
                     Thread* thread,
                     PageTable* page_table,
                     ProcessState state) : m_pid(pid), m_gid(m_pid), m_page_table(page_table),
                                           m_state(state), m_credentials({}) {
      m_threads.add(thread);
    }

    virtual ~Process() = default;

    [[nodiscard]] ProcessState get_state() const { return m_state; }

    [[nodiscard]] ArrayList<Thread*>& get_threads() { return m_threads; }

    ErrorOr<uintptr_t, SysError> handle_open(Userspace<char*> path, u64 flags);
    ErrorOr<uintptr_t, SysError> handle_close(u64);
    ErrorOr<uintptr_t, SysError> handle_dbgln(Userspace<char*> message);
    ErrorOr<uintptr_t, SysError> handle_read(int file_descriptor, Userspace<u8*> buffer, size_t size);
    ErrorOr<uintptr_t, SysError> handle_exit(int exit_code);
    ErrorOr<uintptr_t, SysError> handle_dmesg();
    ErrorOr<uintptr_t, SysError> handle_stats();
    ErrorOr<uintptr_t, SysError> handle_ls();
    ErrorOr<uintptr_t, SysError> handle_mem();

    void map_memory_region(MemoryRegion&);
    void unmap_memory_region(MemoryRegion&);

    [[nodiscard]] PageTable* get_page_table() const { return m_page_table; }

    u64 get_pid() const { return m_pid; }
  };

  class KernelProcess : public Process {
  public:
    explicit KernelProcess(size_t pid,
                           PageTable* page_table,
                           ProcessState state) : Process(pid, nullptr, page_table, state) {
    }
  };

}// namespace Kernel
