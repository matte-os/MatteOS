//
// Created by matejbucek on 13.5.24.
//

#include <Kernel/Sbi/sbi.h>
#include <Kernel/System/Timer.h>
#include <Utils/Assertions.h>

namespace Kernel {
  static Timer* s_timer = nullptr;

  void Timer::init() {
    if(s_timer == nullptr) {
      s_timer = new Timer();
    }
  }

  Timer& Timer::the() {
    runtime_assert(s_timer, "Timer is not initialized");
    return *s_timer;
  }

  void Timer::set_timer(u64 time) {
    SBI::sbi_set_timer(time);
  }
}// namespace Kernel