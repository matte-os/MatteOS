//
// Created by matejbucek on 21.10.22.
//

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

  Process* Scheduler::schedule() {
    auto& process_manager = ProcessManager::the();
    //Someone has the lock, probably kernel.
    //We could probably use RAII to reduce the code.
    auto& lock = process_manager.get_process_list_lock();
    if(!lock.try_lock()) {
      return nullptr;
    }

    const auto& process_list = process_manager.get_process_list();
    process_list->rotate_left();
    auto* first = process_list->first();

    switch(first->get_state()) {
      case ProcessState::Running: {
        //next_trap_frame = first->get_threads()->get(0)->get_trap_frame();
      }; break;
      //FIXME: We should implement SLEEPING threads.
      default: {
      }
    }
    lock.unlock();

    System::get_current_kernel_trap_frame()->current_process = first;

    return first;
  }

  void Scheduler::start_scheduling() {
    Timer::the().set_timer(Timer::DEFAULT_PROCESS_TIME);
    System::the().switch_to_user_mode(schedule()->get_thread()->get_trap_frame());
  }

}// namespace Kernel
