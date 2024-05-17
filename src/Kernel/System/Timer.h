//
// Created by matejbucek on 13.5.24.
//

#pragma once

#include <Utils/Types.h>

namespace Kernel::System {
  class Timer {
  public:
    static void init();
    static Timer& the();

    void set_timer(u64 time);
  };
}// namespace Kernel::System
