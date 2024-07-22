//
// Created by matejbucek on 8.10.22.
//

#pragma once

#include <Kernel/API/Syscall.h>
#include <Kernel/API/SyscallString.h>
#include <Kernel/Arch/riscv64/CPU.h>
#include <Kernel/FileSystem/FileDescriptorTable.h>
#include <Kernel/Memory/MemoryManager.h>
#include <Kernel/Memory/MemoryRegion.h>
#include <Kernel/Memory/PageTable.h>
#include <Kernel/Process/Thread.h>
#include <Kernel/Security/Credentials.h>
#include <Kernel/System/System.h>
#include <Kernel/System/TrapFrame.h>
#include <Utils/Arrays/Array.h>
#include <Utils/Arrays/ArrayList.h>
#include <Utils/Pointers/RefPtr.h>
#include <Utils/Types.h>

using Kernel::System;
using Kernel::MemoryManager;
using Kernel::MemoryRegion;
using Kernel::PageTable;
using Utils::Array;
using Utils::RefPtr;

namespace Kernel {
  enum class ProcessState : u8 {
    Running,
    Sleeping,
    Waiting,
    Dead
  };

  class Process {
  protected:
    size_t m_pid;
    size_t m_gid;
    Thread* m_thread;
    PageTable* m_page_table;
    ProcessState m_state;
    Credentials m_credentials;
    FileDescriptorTable m_fd_table;

  public:
    explicit Process(size_t pid,
                     Thread* thread,
                     PageTable* page_table,
                     ProcessState state) : m_pid(pid), m_gid(m_pid), m_thread(thread), m_page_table(page_table),
                                           m_state(state) {}
    virtual ~Process() = default;
    [[nodiscard]] ProcessState get_state() const { return m_state; }
    [[nodiscard]] Thread* get_thread() const { return m_thread; }

    ErrorOr<uintptr_t, SysError> handle_open(SyscallString path, u64 flags);
    ErrorOr<uintptr_t, SysError> handle_close(u64);

    void map_memory_region(MemoryRegion&);
    void unmap_memory_region(MemoryRegion&);
    [[nodiscard]] PageTable* get_page_table() const { return m_page_table; }
  };

  class KernelProcess : public Process {
  public:
    explicit KernelProcess(size_t pid,
                           PageTable* page_table,
                           ProcessState state) : Process(pid, nullptr, page_table, state) {
    }
  };

}// namespace Kernel
