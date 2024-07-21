#include <Kernel/Arch/riscv64/CPU.h>

namespace Kernel {
  TrapFrame CPU::KERNEL_TRAP_FRAME[8] = {};

  void CPU::halt() {
    asm volatile("wfi");
  }

  SATP CPU::build_satp(SatpMode mode, u64 asid, u64 addr) {
    SATP satp{.ppn = (addr >> 12), .asid = asid, .mode = (u8) mode};
    return satp;
  }

  void CPU::write_satp(u64 value) { asm volatile("csrw satp, %0;" ::"r"(value)); }

  SATP CPU::read_satp() {
    SATP rval;
    asm volatile("csrr %0, satp;" : "=r"(rval));
    return rval;
  }

  void CPU::mscratch_write(u64 value) {
    asm volatile("csrw mscratch, %0" ::"r"(value));
  }

  u64 CPU::mscratch_read() {
    u64 rval;
    asm volatile("csrr %0, mscratch" : "=r"(rval));
    return rval;
  }

  void CPU::sscratch_write(u64 value) {
    asm volatile("csrw sscratch, %0" ::"r"(value));
  }

  u64 CPU::sscratch_read() {
    u64 rval;
    asm volatile("csrr %0, sscratch" : "=r"(rval));
    return rval;
  }

  void CPU::satp_fence(u64 vaddr, u64 asid) {
    asm volatile("sfence.vma %0, %1" ::"r"(vaddr), "r"(asid));
  }

  void CPU::satp_fence_asid(u64 asid) {
    asm volatile("sfence.vma zero, %0" ::"r"(asid));
  }

  void CPU::write_sstatus(u64 sstatus) {
    asm volatile("csrw sstatus, %0" ::"r"(sstatus));
  }

  void CPU::write_stvec(void (*trap_vector)()) {
    asm volatile("csrw stvec, %0" ::"r"(trap_vector));
  }

  void CPU::write_sie(u64 sie) { asm volatile("csrw sie, %0" ::"r"(sie)); }

};// namespace Kernel
