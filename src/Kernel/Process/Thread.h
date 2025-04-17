/**
 * @file Thread.h
 * @author Matěj Bucek
 */
#pragma once

#include <Kernel/System/TrapFrame.h>
#include <Utils/Types.h>
#include <Kernel/Forwards.h>

namespace Kernel {
  enum class ThreadState : u8 {
    Running,
    Sleeping,
    Blocked,
    Dead
  };

  class Thread {
    friend class ProcessManager;
    TrapFrame* m_trap_frame;
    uintptr_t* m_stack;
    size_t m_tid;
    size_t m_pid;
    ThreadState m_state;

  public:
    Thread(TrapFrame* trap_frame, uintptr_t* stack, size_t pid, size_t tid) : m_trap_frame(trap_frame), m_stack(stack), m_pid(pid), m_tid(tid), m_state(ThreadState::Running) {};

    TrapFrame* get_trap_frame() { return m_trap_frame; }
    uintptr_t* get_stack() { return m_stack; }
    size_t get_tid() const { return m_tid; }
    size_t get_pid() const { return m_pid; }
    ThreadState get_state() const { return m_state; }

    ~Thread() = default;
  };
}// namespace Kernel