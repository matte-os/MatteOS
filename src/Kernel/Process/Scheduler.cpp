/**
 * @file Scheduler.cpp
 * @author Matěj Bucek
 */
#include <Kernel/Process/ProcessManager.h>
#include <Kernel/Process/Scheduler.h>
#include <Kernel/System/System.h>
#include <Kernel/System/Timer.h>
#include <Utils/Assertions.h>
#include <Utils/kmalloc.h>

namespace Kernel {
  static Scheduler* s_scheduler = nullptr;

  void Scheduler::init() {
    if(!s_scheduler) {
      s_scheduler = new Scheduler;
    }
  }

  Scheduler& Scheduler::the() {
    runtime_assert(s_scheduler, "Scheduler not initialized.");
    return *s_scheduler;
  }

  Thread* Scheduler::schedule() {
    auto& process_manager = ProcessManager::the();
    //We could probably use RAII to reduce the code.
    auto& lock = process_manager.get_process_list_lock();
    if(!lock.try_lock()) {
      DebugConsole::println("Scheduler: Could not acquire lock.");
      return nullptr;
    }

    auto& run_queue = process_manager.get_run_queue();

    Thread* first = nullptr;
    while(true) {
      run_queue.rotate_left();
      first = run_queue.peek_first();

      if(first->get_state() == ThreadState::Running) {
        break;
      }
    }

    lock.unlock();

    prepare_for_switch(first);

    return first;
  }

  void Scheduler::start_scheduling() {
    Timer::the().set_timer(Timer::DEFAULT_PROCESS_TIME);
    System::the().switch_to_user_mode(schedule()->get_trap_frame());
  }

  void Scheduler::prepare_for_switch(Thread* target_thread) {
    auto* user_tf = target_thread->get_trap_frame();

    // 1. Get the current CPU's context (The "Kernel" side)
    // We need to know: "When this guy traps, where does he land?"
    auto* kernel_tf = System::get_current_kernel_trap_frame();

    // 2. Pack the "Backpack" inside the User TrapFrame

    // B. KERNEL STACK
    // Ideally, every thread has its own Kernel Stack to allow blocking.
    // If you implemented per-thread kernel stacks:
    // If you are using a simple shared per-CPU stack (no kernel preemption):
    user_tf->kernel_sp = kernel_tf->regs[2];

    // C. KERNEL TP (Thread Pointer)
    // This MUST point to the KernelTrapFrame so get_current_kernel_trap_frame() works.
    user_tf->kernel_trap_frame_ptr = reinterpret_cast<u64>(kernel_tf);

    // 3. Update Bookkeeping
    // So we know who is running on this CPU
    kernel_tf->current_process_id = target_thread->get_pid();
    kernel_tf->current_thread_id = target_thread->get_tid();
  }

}// namespace Kernel
