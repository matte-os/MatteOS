//
// Created by matejbucek on 31.8.22.
//

#pragma once

namespace Utils::Locks {
  enum class LockState : u8 {
    UNLOCKED = 0,
    LOCKED = 1
  };

  class SpinLock {
  protected:
    LockState m_state;

  public:
    SpinLock() : m_state(LockState::UNLOCKED) {}
    SpinLock(const SpinLock&) = delete;
    SpinLock& operator=(const SpinLock&) = delete;
    SpinLock(SpinLock&&) = delete;
    SpinLock& operator=(SpinLock&&) = delete;

    void unlock() {
      asm volatile("amoswap.d.rl zero, zero, (%0)" ::"r"(&m_state));
    }

    bool try_lock() {
      LockState st;
      asm volatile("amoswap.d.aq %0, %1, (%2)" : "=r"(m_state) : "r"(1), "r"(this));
      return st == LockState::UNLOCKED;
    }
    ~SpinLock() = default;
  };
}// namespace Utils::Locks
