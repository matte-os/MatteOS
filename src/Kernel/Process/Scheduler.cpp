//
// Created by matejbucek on 21.10.22.
//

#include <Kernel/Process/ProcessManager.h>
#include <Kernel/Process/Scheduler.h>
#include <Kernel/System/System.h>
#include <Utils/kmalloc.h>

namespace Kernel::Process {
  static Scheduler* s_scheduler;

  void Scheduler::init() {
    s_scheduler = new Scheduler;
  }

  Scheduler& Scheduler::the() {
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

    auto& process_list = process_manager.get_process_list();
    process_list->rotate_left();
    auto* first = process_list->first();

    switch(first->get_state()) {
      case ProcessState::RUNNING: {
        //next_trap_frame = first->get_threads()->get(0)->get_trap_frame();
      }; break;
      //FIXME: We should implement SLEEPING threads.
      default: {
      }
    }
    lock.unlock();
    return first;
  }

  void Scheduler::start_scheduling() {
    System::System::the().switch_to_user_mode(schedule()->get_thread()->get_trap_frame());
  }

}// namespace Kernel::Process
