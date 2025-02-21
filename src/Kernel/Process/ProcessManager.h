//
// Created by matejbucek on 1.9.22.
//

#pragma once

#include <Kernel/Process/Process.h>
#include <Utils/Arrays/Queue.h>
#include <Utils/Locks/Lock.h>
#include <Kernel/Locking/Locked.h>

using Utils::LinkedQueue;
using Utils::Locks::SpinLock;

namespace Kernel {
class ProcessManager {
  static constexpr size_t START_ADDRESS = 0x20000000;
  static constexpr size_t STACK_ADDRESS = 0xf0000000;
  static constexpr size_t STACK_PAGES = 2;
  static constexpr size_t THREAD_FRAME_ADDRESS = 0x1000;
  RefPtr<LinkedQueue<Process *>> m_processes;
  SpinLock m_process_list_lock;
  KernelProcess *m_kernel_process;
  size_t m_pid_counter;

public:
  static void init(PageTable *kernel_root_table);
  static ProcessManager &the();
  Process &initialize_process();
  RefPtr<LinkedQueue<Process *>> &get_process_list() { return m_processes; }
  SpinLock &get_process_list_lock() { return m_process_list_lock; }
  KernelProcess &get_kernel_process() const { return *m_kernel_process; }
  PageTable *create_dummy_process(uintptr_t program_counter);
  void create_initial_processes();
  ~ProcessManager();

private:
  explicit ProcessManager(PageTable *kernel_root_table);
  KernelProcess &initialize_kernel_process(PageTable *kernel_root_table);
  Thread *initialize_thread(uintptr_t program_counter, SATP satp,
                            size_t hart_id);
};
} // namespace Kernel
