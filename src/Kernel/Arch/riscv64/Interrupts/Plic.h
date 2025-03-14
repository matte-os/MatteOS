#pragma once

#include <Utils/Optional.h>
#include <Utils/Types.h>

using Utils::Optional;

namespace Kernel {
  /**
  * This PLIC implementation is based on the SiFive PLIC.
  * https://sifive.cdn.prismic.io/sifive%2F834354f0-08e6-423c-bf1f-0cb58ef14061_fu540-c000-v1.0.pdf
  * */
  class Plic {
    enum class PlicOffsets : size_t {
      Priority = 0x000000,
      Pending = 0x001000,
      InterruptEnable = 0x002000, // Interrupt enables for M-mode
      PriorityThreshold = 0x200000, // Priority threshold for M-mode
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
    void complete(u32 context_id, u32 id);
    bool is_pending(u32 id);

  private:
    Plic();
  };
};// namespace Kernel
