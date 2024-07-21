//
// Created by matejbucek on 1.9.22.
//

#include <Kernel/Arch/riscv64/CPU.h>
#include <Kernel/Memory/MemoryManager.h>
#include <Kernel/Process/ProcessManager.h>
#include <Kernel/Sbi/sbi.h>
#include <Kernel/System/DeviceManager.h>
#include <Kernel/System/System.h>
#include <Kernel/System/Trap.h>
#include <Kernel/VirtIO/VirtIODeviceIDs.h>
#include <Utils/Assertions.h>
#include <Utils/DebugConsole.h>

using Kernel::Memory::EntryBits;

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
      kernel_trap_frame->regs[2] = reinterpret_cast<u64>(MemoryManager::the().zalloc(1));
      if(i == 0) {
        MemoryManager::the().map_range(*MemoryManager::the().get_current_root_page_table(), TRAP_VECTOR_ADDRESS, TRAP_VECTOR_ADDRESS + 0x1000, reinterpret_cast<uintptr_t>(kernel_trap_frame), static_cast<u64>(EntryBits::READ_WRITE));
        kernel_trap_frame->satp = CPU::read_satp();
      } else {
        auto* root_page_table = reinterpret_cast<PageTable*>(MemoryManager::the().zalloc(1));
        MemoryManager::the().map_system_defaults(*root_page_table);
        MemoryManager::the().map_range(*root_page_table, TRAP_VECTOR_ADDRESS, TRAP_VECTOR_ADDRESS + 0x1000, reinterpret_cast<uintptr_t>(kernel_trap_frame), static_cast<u64>(EntryBits::READ_WRITE));
        kernel_trap_frame->satp = CPU::build_satp(SatpMode::Sv39, 0, reinterpret_cast<uintptr_t>(root_page_table));
      }
      m_kernel_trap_frames.add(kernel_trap_frame);
    }
  }

  void System::parse_fdt(FDTHeader* header) {
    if(*header->magic == 0xd00dfeed) {

      auto fdt_version = *header->version;
      auto total_size = *header->totalsize;

      Utils::DebugConsole::print("System: FDT Version ");
      Utils::DebugConsole::print_number(fdt_version, 10);
      Utils::DebugConsole::print(" Total Size ");
      Utils::DebugConsole::print_number(total_size, 10);
      Utils::DebugConsole::println(".");
      m_fdt_parser = new FDTParser();
      m_fdt_parser->parse(header);

      auto result = m_fdt_parser->find_node(String("/memory"));
      if(result.has_error()) {
        DebugConsole::println("System: Could not find /memory node in FDT.");
      } else {
        DebugConsole::println("System: Found /memory node in FDT.");
      }

      auto virtio_result = m_fdt_parser->find_nodes("/soc/virtio_mmio");

      if(virtio_result.has_error()) {
        DebugConsole::println("System: Could not find /soc/virtio_mmio node in FDT.");
      } else {
        auto virtio_nodes = virtio_result.get_value();
        DebugConsole::println("System: Found /soc/virtio_mmio node in FDT.");
        DebugConsole::print("Found node count: ");
        DebugConsole::print_ln_number(virtio_nodes.size(), 10);

        for(size_t i = 0; i < virtio_nodes.size(); i++) {
          auto node = virtio_nodes[i];
          auto string_address = node->get_address();
          auto address = string_address.to_uint(16);
          auto result = DeviceManager::the().try_to_load_mmio_device(address);
          if(result.has_error()) {
            DebugConsole::println("System: Could not load MMIO device.");
          } else {
            DebugConsole::println("System: Loaded MMIO device.");
            DebugConsole::print("System: The device type is: ");
            DebugConsole::print_ln_number(static_cast<size_t>(result.get_value()->get_device_type()), 10);
            auto init_or_error = result.get_value()->init();
            if(init_or_error.has_error()) {
              DebugConsole::print("System: Could not initialise device. Error: ");
              DebugConsole::println(init_or_error.get_error().get_message().value());
            } else {
              DebugConsole::println("System: Initialised device.");
            }
          }
        }
      }
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
    CPU::write_sstatus((0b01 << 11) | (1 << 7) | (1 << 5));
    CPU::write_stvec(trap_vector);
    CPU::write_sie(0x888);
  }

  void System::set_default_trap_vector() {
    set_trap_vector(m_trap_vector);
  }

  void System::setup_interrupts() {
    set_default_trap_vector();
  }

  void System::switch_to_user_mode(TrapFrame* frame_to_apply) {
    DebugConsole::println("About switching to usermode");
    DebugConsole::print_ln_number(MY_OFFSETOF(TrapFrame, program_counter), 10);
    DebugConsole::print_ln_number(frame_to_apply->program_counter, 16);
    switch_to_user(frame_to_apply);
  }
}// namespace Kernel
