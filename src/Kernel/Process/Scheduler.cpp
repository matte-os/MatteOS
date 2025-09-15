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

    System::get_current_kernel_trap_frame()->current_process_id = first->get_pid();
    System::get_current_kernel_trap_frame()->current_thread_id = first->get_tid();

    return first;
  }

  void Scheduler::start_scheduling() {
    Timer::the().set_timer(Timer::DEFAULT_PROCESS_TIME);
    System::the().switch_to_user_mode(schedule()->get_trap_frame());
  }

}// namespace Kernel
