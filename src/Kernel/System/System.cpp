/**
 * @file System.cpp
 * @author Matěj Bucek
 */
#include <Kernel/Arch/riscv64/CPU.h>
#include <Kernel/Devices/Console/ConsoleDevice.h>
#include <Kernel/Devices/DeviceManager.h>
#include <Kernel/Firmware/DeviceTree.h>
#include <Kernel/Memory/MemoryManager.h>
#include <Kernel/Process/ProcessManager.h>
#include <Kernel/SBI/SBI.h>
#include <Kernel/System/System.h>
#include <Kernel/System/TrapFrame.h>
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
      hart_init(0);
    }
  }

  System& System::the() {
    runtime_assert(s_system, "System is not initialized.");
    return *s_system;
  }

  System::System() {
    m_number_of_harts = 0;
    init_harts();
    //Utils::DebugConsole::println("System: Initialised.");

    m_kernel_trap_frames = ArrayList<KernelTrapFrame*>();
    auto master_kernel_satp = CPU::read_satp();

    for(size_t i = 0; i < m_number_of_harts; i++) {
      auto* kernel_trap_frame = reinterpret_cast<KernelTrapFrame*>(MemoryManager::the().zalloc(1));
      auto* hart_stack_bottom = MemoryManager::the().zalloc(2);
      auto hart_stack_top = reinterpret_cast<u64>(hart_stack_bottom) + 4096 * 2;

      kernel_trap_frame->cpu_id = i;
      kernel_trap_frame->regs[2] = hart_stack_top;
      kernel_trap_frame->kernel_sp = hart_stack_top;
      kernel_trap_frame->kernel_trap_frame_ptr = reinterpret_cast<u64>(kernel_trap_frame);
      kernel_trap_frame->kernel_satp = *reinterpret_cast<u64*>(&master_kernel_satp);

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

  extern "C" void hart_init(u32 cpu_id) {
    auto* trap_frame = System::the().get_kernel_trap_frame(cpu_id);

    CPU::sscratch_write(reinterpret_cast<u64>(trap_frame));
    asm volatile("mv tp, %0" ::"r"(trap_frame));
  }
}// namespace Kernel
