//
// Created by matejbucek on 31.8.22.
//

#pragma once

namespace Utils::Locks {
  class SpinLock {
  protected:
    volatile u64 m_state;

  public:
    SpinLock() : m_state(0) {}
    SpinLock(const SpinLock&) = delete;
    SpinLock& operator=(const SpinLock&) = delete;
    SpinLock(SpinLock&&) = delete;
    SpinLock& operator=(SpinLock&&) = delete;

    void unlock() {
      asm volatile("amoswap.d.rl zero, zero, (%0)" ::"r"(&m_state) : "memory");
    }

    bool try_lock() {
      u64 st;
      asm volatile("amoswap.d.aq %0, %1, (%2)" : "=r"(st) : "r"(1), "r"(&m_state) : "memory");
      return st == 0;
    }
    ~SpinLock() = default;
  };
}// namespace Utils::Locks
