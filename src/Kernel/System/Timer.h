//
// Created by matejbucek on 13.5.24.
//

#pragma once

#include <Utils/Types.h>

namespace Kernel {
  class Timer {
    u32 m_frequency;
  public:
    explicit Timer(const u32 frequency) : m_frequency(frequency) {}
    static constexpr size_t DEFAULT_PROCESS_TIME = 1000; // In ms
    static void init();
    static Timer& the();

    void set_timer(u64 time_in_ms) const;
  };
}// namespace Kernel::System
