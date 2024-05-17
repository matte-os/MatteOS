//
// Created by matejbucek on 1.9.22.
//

#pragma once

#include <Kernel/Firmware/FDT/fdt.h>
#include <Kernel/Memory/MemoryRegion.h>
#include <Kernel/Sbi/sbi.h>
#include <Kernel/System/TrapFrame.h>
#include <Utils/Arrays/ArrayList.h>
#include <Utils/Pointers/RefPtr.h>
#include <Utils/Types.h>

using Kernel::Memory::MemoryRegion;
using Utils::ArrayList;
using Utils::Pointers::RefPtr;

namespace Kernel::System {
  struct SStatus {
    u64 wpri1 : 1;
    u64 sie   : 1;
    u64 wpri2 : 3;
    u64 spie  : 1;
    u64 ube   : 1;
    u64 wpri3 : 1;
    u64 spp   : 1;
    u64 vs    : 2;
    u64 wpri4 : 2;
    u64 fs    : 2;
    u64 xs    : 2;
    u64 wpri5 : 1;
    u64 sum   : 1;
    u64 mxr   : 1;
    u64 wpri6 : 12;
    u64 uxl   : 2;
    u64 wpri7 : 29;
    u64 sd    : 1;
  };

  struct Stvec {
    u64 mode : 2;
    u64 warl : 62;
  };

  class System {
  private:
    size_t m_number_of_harts;
    RefPtr<ArrayList<MemoryRegion>> m_memory_regions;
    u64* m_mtime;

  public:
    static void init();
    static System& the();
    void parse_fdt(fdt_header* header);
    [[nodiscard]] size_t get_number_of_harts() const;
    [[nodiscard]] u64 get_mtime() const { return *m_mtime; }
    void set_default_trap_vector();
    //NOTE: This function is called by individual harts to initialize the kernel process for them.
    void setup_interrupts();
    [[noreturn]] void switch_to_user_mode(TrapFrame* frame_to_apply);

  private:
    System();
    ~System() = default;
    void init_harts();
    void set_trap_vector(void (*trap_vector)());
  };

}// namespace Kernel::System