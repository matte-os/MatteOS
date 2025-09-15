/**
 * @file Timer.h
 * @author Matěj Bucek
 */
#pragma once

#include <Utils/Types.h>

namespace Kernel {
  class Timer {
    u32 m_frequency;
  public:
    explicit Timer(const u32 frequency) : m_frequency(frequency) {}
    static constexpr size_t DEFAULT_PROCESS_TIME = 100; // In ms
    static void init();
    static Timer& the();

    void set_timer(u64 time_in_ms) const;
    u64 current_time_in_ms() const;
  };
}// namespace Kernel::System
