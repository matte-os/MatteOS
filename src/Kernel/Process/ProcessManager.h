//
// Created by matejbucek on 1.9.22.
//

#pragma once

#include <Kernel/Locking/Locked.h>
#include <Kernel/Process/Process.h>
#include <Utils/Arrays/Queue.h>
#include <Utils/Locks/Lock.h>

using Utils::LinkedQueue;
using Utils::Locks::SpinLock;

namespace Kernel {
  class ProcessManager {
    static constexpr size_t START_ADDRESS = 0x20000000;
    static constexpr size_t STACK_ADDRESS = 0xf0000000;
    static constexpr size_t STACK_PAGES = 2;
    static constexpr size_t THREAD_FRAME_ADDRESS = 0x1000;
    LinkedQueue<Thread*> m_run_queue;
    ArrayList<Process*> m_process_list;
    SpinLock m_process_list_lock;
    KernelProcess* m_kernel_process;
    size_t m_pid_counter;

  public:
    static void init(PageTable* kernel_root_table);
    static ProcessManager& the();
    Process& initialize_process();

    LinkedQueue<Thread*>& get_run_queue() { return m_run_queue; }

    SpinLock& get_process_list_lock() { return m_process_list_lock; }

    KernelProcess& get_kernel_process() const { return *m_kernel_process; }

    PageTable* create_dummy_process(uintptr_t program_counter);
    void create_initial_processes();
    ~ProcessManager();

    ErrorOr<Process*> get_process(u64 pid);
    ErrorOr<Thread*> get_thread(u64 pid, u64 tid);

    void unblock(size_t pid, size_t tid);
    void block(size_t pid, size_t tid);

    Thread* get_current_thread();
    Process* get_current_process();

  private:
    explicit ProcessManager(PageTable* kernel_root_table);
    KernelProcess& initialize_kernel_process(PageTable* kernel_root_table);
    Thread* initialize_thread(uintptr_t program_counter, SATP satp,
                              size_t hart_id, size_t pid, size_t tid);
  };
}// namespace Kernel
