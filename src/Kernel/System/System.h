/**
 * @file System.h
 * @author Matěj Bucek
 */
#pragma once

#include <Kernel/Arch/riscv64/CSR.h>
#include <Kernel/Firmware/FDT/FDTParser.h>
#include <Kernel/Memory/VirtualMemory/MemoryRegion.h>
#include <Kernel/System/KernelTrapFrame.h>
#include <Kernel/System/TrapFrame.h>
#include <Utils/Arrays/ArrayList.h>
#include <Utils/Pointers/RefPtr.h>
#include <Utils/Types.h>

using Kernel::FDTParser;
using Kernel::MemoryRegion;
using Utils::ArrayList;
using Utils::RefPtr;

namespace Kernel {
  class System {
    size_t m_number_of_harts;
    ArrayList<MemoryRegion> m_memory_regions;
    ArrayList<KernelTrapFrame*> m_kernel_trap_frames;
    static constexpr size_t TRAP_VECTOR_ADDRESS = 0x1000;

  public:
    static void init();
    static System& the();

    static KernelTrapFrame* get_current_kernel_trap_frame() {
      KernelTrapFrame* tf;
      asm volatile("mv %0, tp" : "=r"(tf));
      return tf;
    }

    KernelTrapFrame* get_kernel_trap_frame(size_t hart_id) { return m_kernel_trap_frames[hart_id]; }

    [[nodiscard]] ArrayList<MemoryRegion> get_memory_regions() const { return m_memory_regions; }

    [[nodiscard]] size_t get_number_of_harts() const;

    [[nodiscard]] u64 get_time() const {
      return Kernel::RISCV64::CSR::read<RISCV64::CSR::Address::TIME>();
    }

    void set_default_trap_vector();
    //NOTE: This function is called by individual harts to initialize the kernel process for them.
    void setup_interrupts();
    [[noreturn]] void switch_to_user_mode(TrapFrame* frame_to_apply);
    void install_from_device_tree();

  private:
    System();
    ~System() = default;
    void init_harts();
    void set_trap_vector(void (*trap_vector)());
  };

  extern "C" void hart_init(u32 cpu_id);

}// namespace Kernel