#include <Kernel/Arch/riscv64/Interrupts/Plic.h>
#include <Kernel/Firmware/DeviceTree.h>
#include <Kernel/Logger.h>
#include <Utils/Assertions.h>
#include <Utils/DebugConsole.h>

namespace Kernel {
  static Plic* s_plic = nullptr;

  using Utils::DebugConsole;

  Plic& Plic::the() {
    runtime_assert(s_plic, "Plic not initialized");
    return *s_plic;
  }

  void Plic::init() {
    if(!s_plic) {
      s_plic = new Plic();
    } else {
      dbgln("Plic: Already initialized!");
    }
  }

  Plic::Plic() {
    dbgln("Plic: Initializing...");
    dbgln("Plic: Detecting base address from the device tree");
    auto node_or_error = DeviceTree::the().find_node("/soc/plic");
    if(node_or_error.has_error()) {
      dbgln("Plic: Could not find the plic node in the device tree ({})", "/soc/plic");
      return;
    }

    auto node = node_or_error.get_value();
    m_base = node->get_address().to_uint(16);
    dbgln("Plic: Base: {16}", m_base);
  }

  void Plic::enable(u32 context_id, u32 id) {
    dbgln("Plic: Enable: context_id: {}, id: {}", context_id, id);
    auto* enable = reinterpret_cast<u32*>(m_base + static_cast<size_t>(PlicOffsets::InterruptEnable) + 4 * (id / 32) + 0x80 * context_id);
    u32 actual_id = 1 << id % 32;
    *enable = *enable | actual_id;

    //DebugConsole::print("Plic: Enable write to ");
    //DebugConsole::print_number(reinterpret_cast<u64>(enable), 16);
    //DebugConsole::print(" value ");
    //DebugConsole::print_ln_number(*enable, 16);
  }

  void Plic::disable(u32 context_id, u32 id) {
    auto* enable = reinterpret_cast<u32*>(m_base + static_cast<size_t>(PlicOffsets::InterruptEnable) + 4 * (id / 32) + 0x80 * context_id);
    u32 actual_id = 1 << id % 32;
    *enable = *enable & ~actual_id;
  }

  void Plic::set_priority(u32 id, u8 priority) {
    dbgln("Plic: Setting priority: id: {}, priority: {}", id, priority);
    auto* priority_register = reinterpret_cast<u32*>(m_base + static_cast<size_t>(PlicOffsets::Priority) + 4 * id);
    *priority_register = priority & 7;
  }

  void Plic::set_threshold(u32 context_id, u8 threshold) {
    dbgln("Plic: Setting threshold: id: {}, threshold: {}", context_id, threshold);
    auto* threshold_register = reinterpret_cast<u32*>(m_base + static_cast<size_t>(PlicOffsets::PriorityThreshold) + 0x1000 * context_id);
    *threshold_register = threshold & 7;
  }

  Optional<u32> Plic::next(u32 context_id) {
    auto* claim_register = reinterpret_cast<u32*>(m_base + static_cast<size_t>(PlicOffsets::Claim) + 0x1000 * context_id);
    u32 claim = *claim_register;
    if(!claim) {
      return {};
    }

    return {claim};
  }

  void Plic::complete(u32 context_id, u32 id) {
    auto* complete_register = reinterpret_cast<u32*>(m_base + static_cast<size_t>(PlicOffsets::Complete) + 0x1000 * context_id);
    *complete_register = id;

    //dbgln("Plic: Completed id: {}", id);
  }

  bool Plic::is_pending(u32 id) {
    // The id/32 is the index of the register
    // The 4 is the size of the register in bytes, so we multiply by that
    // to get the memory address of the register
    auto* pending = reinterpret_cast<u32*>(m_base + static_cast<size_t>(PlicOffsets::Pending) + 4 * (id / 32));
    // The actual id is the bit in the register
    u32 actual_id = 1 << id % 32;
    return (*pending & actual_id) != 0;
  }

};// namespace Kernel