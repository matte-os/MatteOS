#include <Kernel/Arch/riscv64/Interrupts/Plic.h>
#include <Utils/DebugConsole.h>
#include <Utils/Assertions.h>

namespace Kernel {
  static Plic* s_plic = nullptr;

  Plic& Plic::the() {
    runtime_assert(s_plic, "Plic not initialized");
    return *s_plic;
  }

  void Plic::init(size_t, size_t, size_t, size_t, size_t) {
    s_plic = new Plic();
  }

  void Plic::enable(u32 id) {
    u32* enables = (u32*) PLIC_INT_ENABLE;
    u32 actualId = 1 << id;
    *enables = *enables | actualId;
    Utils::DebugConsole::print("Enabled: ");
    Utils::DebugConsole::print_ln_number(*enables, 2);
  }

  void Plic::set_priority(u32 id, u8 priority) {
    u32 actualPriority = ((u32) priority) & 7;
    u32* priorityRegister = (u32*) PLIC_PRIORITY;
    *(priorityRegister + id) = actualPriority;
    Utils::DebugConsole::print("Priority: ");
    Utils::DebugConsole::print_ln_number(*(priorityRegister + 4 * id), 2);
  }

  void Plic::set_treshold(u8 treshold) {
    u32 actualTreshold = treshold & 7;
    u32* tresholdRegister = (u32*) PLIC_TRESHOLD;
    *tresholdRegister = actualTreshold;
  }

  Optional<u32> Plic::next() {
    u32* claimRegister = (u32*) PLIC_CLAIM;
    u32 claimNo = *claimRegister;
    if(claimNo == 0) {
      return Optional<u32>();
    }

    return Optional<u32>(claimNo);
  }

  void Plic::complete(u32 id) {
    u32* completeRegister = (u32*) PLIC_CLAIM;
    *completeRegister = id;
  }

  bool Plic::isPending(u32 id) {
    u32* pending = (u32*) PLIC_PENDING;
    u32 actualId = 1 << id;
    u32 pendingIds = *pending;
    return actualId & pendingIds != 0;
  }
};// namespace Kernel