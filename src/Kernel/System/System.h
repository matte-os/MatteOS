//
// Created by matejbucek on 1.9.22.
//

#pragma once

#include <Kernel/Firmware/FDT/FDT.h>
#include <Kernel/Firmware/FDT/FDTParser.h>
#include <Kernel/Memory/MemoryRegion.h>
#include <Kernel/SBI/SBI.h>
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
  private:
    size_t m_number_of_harts;
    RefPtr<ArrayList<MemoryRegion>> m_memory_regions;
    u64* m_mtime;
    ArrayList<KernelTrapFrame*> m_kernel_trap_frames;
    static const size_t TRAP_VECTOR_ADDRESS = 0x1000;

  public:
    static void init();
    static System& the();
    static KernelTrapFrame* get_current_kernel_trap_frame() { return reinterpret_cast<KernelTrapFrame*>(TRAP_VECTOR_ADDRESS); }
    KernelTrapFrame* get_kernel_trap_frame(size_t hart_id) { return m_kernel_trap_frames[hart_id]; }
    [[nodiscard]] RefPtr<ArrayList<MemoryRegion>> get_memory_regions() const { return m_memory_regions; }
    [[nodiscard]] size_t get_number_of_harts() const;
    [[nodiscard]] u64 get_mtime() const { return *m_mtime; }
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

}// namespace Kernel