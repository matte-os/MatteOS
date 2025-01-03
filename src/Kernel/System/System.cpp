//
// Created by matejbucek on 1.9.22.
//

#include <Kernel/Arch/riscv64/CPU.h>
#include <Kernel/Devices/Console/ConsoleDevice.h>
#include <Kernel/Devices/DeviceManager.h>
#include <Kernel/Firmware/DeviceTree.h>
#include <Kernel/Memory/MemoryManager.h>
#include <Kernel/Process/ProcessManager.h>
#include <Kernel/SBI/SBI.h>
#include <Kernel/System/System.h>
#include <Kernel/System/TrapFrame.h>
#include <Kernel/Uart.h>
#include <Utils/Assertions.h>
#include <Utils/DebugConsole.h>

using Kernel::EntryBits;
using Kernel::TrapFrame;

extern "C" void m_trap_vector();
extern "C" void switch_to_user(TrapFrame* trap_frame);

#define MY_OFFSETOF(type, member) ((size_t) (&((type*) 0)->member))

namespace Kernel {
  static System* s_system = nullptr;

  void System::init() {
    if(!s_system) {
      s_system = new System;
    }
  }

  System& System::the() {
    runtime_assert(s_system, "System is not initialized.");
    return *s_system;
  }

  System::System() {
    m_memory_regions = RefPtr<ArrayList<MemoryRegion>>(new ArrayList<MemoryRegion>());
    m_number_of_harts = 0;
    init_harts();
    m_mtime = (u64*) 0x0200BFF8;
    //Utils::DebugConsole::println("System: Initialised.");

    m_kernel_trap_frames = ArrayList<KernelTrapFrame*>();
    for(size_t i = 0; i < m_number_of_harts; i++) {
      auto* kernel_trap_frame = reinterpret_cast<KernelTrapFrame*>(MemoryManager::the().zalloc(1));
      kernel_trap_frame->cpu_id = i;
      kernel_trap_frame->regs[2] = MemoryManager::the().get_stack_start();
      DebugConsole::print("System: TrapFrame address: ");
      DebugConsole::print_ln_number(reinterpret_cast<u64>(kernel_trap_frame), 16);

      DebugConsole::print("System: TrapFrame regs[2] address: ");
      DebugConsole::print_ln_number(kernel_trap_frame->regs[2], 16);
      if(i == 0) {
        MemoryManager::the().map_range(*MemoryManager::the().get_current_root_page_table(), TRAP_VECTOR_ADDRESS, TRAP_VECTOR_ADDRESS + 0x1000, reinterpret_cast<uintptr_t>(kernel_trap_frame), static_cast<u64>(EntryBits::READ_WRITE));
        kernel_trap_frame->satp = CPU::read_satp();
      } else {
        // The stack decreases from the top of the memory
        auto* root_page_table = reinterpret_cast<PageTable*>(MemoryManager::the().zalloc(4) + 0x4000);
        MemoryManager::the().map_system_defaults(*root_page_table);
        MemoryManager::the().map_range(*root_page_table, TRAP_VECTOR_ADDRESS, TRAP_VECTOR_ADDRESS + 0x1000, reinterpret_cast<uintptr_t>(kernel_trap_frame), static_cast<u64>(EntryBits::READ_WRITE));
        kernel_trap_frame->satp = CPU::build_satp(SatpMode::Sv39, 0, reinterpret_cast<uintptr_t>(root_page_table));
      }
      m_kernel_trap_frames.add(kernel_trap_frame);
    }
  }

  size_t System::get_number_of_harts() const {
    return m_number_of_harts;
  }

  void System::init_harts() {
    //FIXME: We should get this information out of a fdt. This is a fast hack to get it in dirty way.
    for(int i = 0; i < 100; i++) {
      auto hart = SBI::sbi_hart_get_status(i);
      if(hart.error == -3) {
        Utils::DebugConsole::print("System: sbi_hart_get_status returned ");
        Utils::DebugConsole::print_ln_number(hart.error, 10);
        break;
      } else {
        m_number_of_harts++;
      }
    }
    Utils::DebugConsole::print("System: The system has ");
    Utils::DebugConsole::print_number(m_number_of_harts, 10);
    Utils::DebugConsole::println(" harts.");
  }

  void System::set_trap_vector(void(trap_vector)()) {
    //CPU::write_sstatus((0b01 << 11) | (1 << 7) | (1 << 5));
    CPU::write_stvec(trap_vector);
    //CPU::write_sie(0x888);
  }

  void System::set_default_trap_vector() {
    set_trap_vector(m_trap_vector);
  }

  void System::setup_interrupts() {
    set_default_trap_vector();
  }

  void System::switch_to_user_mode(TrapFrame* frame_to_apply) {
    switch_to_user(frame_to_apply);
  }

  void System::install_from_device_tree() {
    auto& device_tree = DeviceTree::the();
    auto result = device_tree.find_node(String("/memory"));
    if(result.has_error()) {
      DebugConsole::println("System: Could not find /memory node in FDT.");
    } else {
      DebugConsole::println("System: Found /memory node in FDT.");
    }
  }
}// namespace Kernel
