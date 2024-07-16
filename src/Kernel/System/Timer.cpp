//
// Created by matejbucek on 13.5.24.
//

#include <Kernel/Sbi/sbi.h>
#include <Kernel/System/Timer.h>

namespace Kernel {
  static Timer* g_timer = nullptr;

  void Timer::init() {
    if(g_timer == nullptr) {
      g_timer = new Timer();
    }
  }

  Timer& Timer::the() {
    return *g_timer;
  }

  void Timer::set_timer(u64 time) {
    SBI::sbi_set_timer(time);
  }
}// namespace Kernel