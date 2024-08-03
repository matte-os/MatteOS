#pragma once

#include <Utils/Optional.h>
#include <Utils/Types.h>

using Utils::Optional;

namespace Kernel {
  class Plic {
  private:
    enum class PlicOffsets : size_t {
      Priority = 0x000000,
      Pending = 0x001000,
      InterruptEnable = 0x002000,
      PriorityThreshold = 0x200000,
      Claim = 0x200004,
      Complete = 0x200004,
    };
    size_t m_base;

  public:
    static Plic& the();
    static void init();
    void enable(u32 context_id, u32 id);
    void disable(u32 context_id, u32 id);
    void set_priority(u32 id, u8 priority);
    void set_threshold(u32 context_id, u8 threshold);
    Optional<u32> next(u32 context_id);
    void complete(u32 context_id);
    bool is_pending(u32 id);

  private:
    Plic();
  };
};// namespace Kernel
