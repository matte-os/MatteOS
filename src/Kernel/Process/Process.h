//
// Created by matejbucek on 8.10.22.
//

#pragma once

#include <Kernel/CPU.h>
#include <Kernel/Memory/MemoryManager.h>
#include <Kernel/Memory/MemoryRegion.h>
#include <Kernel/Memory/PageTable.h>
#include <Kernel/Process/Thread.h>
#include <Kernel/System/System.h>
#include <Kernel/System/TrapFrame.h>
#include <Utils/Arrays/Array.h>
#include <Utils/Arrays/ArrayList.h>
#include <Utils/Pointers/RefPtr.h>
#include <Utils/Types.hh>

using Kernel::Memory::MemoryManager;
using Kernel::Memory::MemoryRegion;
using Kernel::Memory::PageTable;
using Kernel::System::System;
using Utils::Array;
using Utils::Pointers::RefPtr;

namespace Kernel::Process {
  enum class ProcessState : u8 {
    RUNNING,
    SLEEPING,
    WAITING,
    DEAD
  };

  class Process {
  protected:
    size_t m_pid;
    size_t m_gid;
    Thread* m_thread;
    PageTable* m_page_table;
    ProcessState m_state;

  public:
    explicit Process(size_t pid,
                     Thread* thread,
                     PageTable* page_table,
                     ProcessState state) : m_pid(pid), m_gid(m_pid), m_thread(thread), m_page_table(page_table),
                                           m_state(state) {}
    virtual ~Process() = default;
    [[nodiscard]] ProcessState get_state() const { return m_state; }
    [[nodiscard]] Thread* get_thread() const { return m_thread; }
    void map_memory_region(MemoryRegion&);
    void unmap_memory_region(MemoryRegion&);
    [[nodiscard]] PageTable* get_page_table() const { return m_page_table; }
  };

  class KernelProcess : public Process {
  protected:
    Array<KernelTrapFrame>* m_kernel_trap_frames;

  public:
    explicit KernelProcess(size_t pid,
                           PageTable* page_table,
                           ProcessState state) : Process(pid, nullptr, page_table, state) {
      auto number_of_harts = System::System::the().get_number_of_harts();
      auto number_of_pages = MemoryManager::PAGE_SIZE / sizeof(KernelTrapFrame);
      m_kernel_trap_frames = new Array<KernelTrapFrame>(number_of_harts, reinterpret_cast<KernelTrapFrame*>(MemoryManager::the().zalloc(number_of_pages)));
      auto& this_threads_frame = m_kernel_trap_frames->get(0);
      this_threads_frame.cpu_id = 0;
      this_threads_frame.satp = CPU::build_satp(SatpMode::Sv39, 0, reinterpret_cast<uintptr_t>(page_table));
      this_threads_frame.regs[2] = reinterpret_cast<u64>(MemoryManager::the().zalloc(1));
    }

    [[nodiscard]] auto get_kernel_trap_frames() const {
      return m_kernel_trap_frames;
    }
  };

}// namespace Kernel::Process
