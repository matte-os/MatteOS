#include <Kernel/Arch/riscv64/Interrupts/Plic.h>
#include <Kernel/Firmware/DeviceTree.h>
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
      DebugConsole::println("Plic: Already initialized.");
    }
  }

  Plic::Plic() {
    DebugConsole::println("Plic: Initializing");
    DebugConsole::println("Plic: Detecting base address from the device tree");
    auto node_or_error = DeviceTree::the().find_node("/soc/plic");
    if(node_or_error.has_error()) {
      DebugConsole::println("Plic: Could not find the plic node in the device tree");
      return;
    }

    auto node = node_or_error.get_value();
    m_base = node->get_address().to_uint(16);
    DebugConsole::print("Plic: Base address: ");
    DebugConsole::print_ln_number(m_base, 16);
  }

  void Plic::enable(u32 context_id, u32 id) {
    DebugConsole::print("Plic: Enabling interrupt ");
    DebugConsole::print_number(id, 10);
    DebugConsole::print(" for context ");
    DebugConsole::print_ln_number(context_id, 10);
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
    DebugConsole::print("Plic: Setting priority ");
    DebugConsole::print_number(priority, 10);
    DebugConsole::print(" for interrupt ");
    DebugConsole::print_ln_number(id, 10);
    auto* priority_register = reinterpret_cast<u32*>(m_base + static_cast<size_t>(PlicOffsets::Priority) + 4 * id);
    *priority_register = priority & 7;

    //DebugConsole::print("Plic: Priority write to ");
    //DebugConsole::print_number(reinterpret_cast<u64>(priority_register), 16);
    //DebugConsole::print(" value ");
    //DebugConsole::print_ln_number(*priority_register, 16);
  }

  void Plic::set_threshold(u32 context_id, u8 threshold) {
    DebugConsole::print("Plic: Setting threshold ");
    DebugConsole::print_number(threshold, 10);
    DebugConsole::print(" for context ");
    DebugConsole::print_ln_number(context_id, 10);
    auto* threshold_register = reinterpret_cast<u32*>(m_base + static_cast<size_t>(PlicOffsets::PriorityThreshold) + 0x1000 * context_id);
    *threshold_register = threshold & 7;

    //DebugConsole::print("Plic: Threshold write to ");
    //DebugConsole::print_number(reinterpret_cast<u64>(threshold_register), 16);
    //DebugConsole::print(" value ");
    //DebugConsole::print_ln_number(*threshold_register, 16);
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

    DebugConsole::print("Plic: Completed interrupt for context ");
    DebugConsole::print_ln_number(context_id, 10);

    //DebugConsole::print("Plic: Complete write to ");
    //DebugConsole::print_number(reinterpret_cast<u64>(complete_register), 16);
    //DebugConsole::print(" value ");
    //DebugConsole::print_ln_number(id, 16);
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