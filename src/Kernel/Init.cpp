#define KERNEL 1

#include <Kernel/Arch/riscv//CPU.h>
#include <Kernel/Arch/riscv/Satp.h>
#include <Kernel/Firmware/FDT/fdt.h>
#include <Kernel/Memory/KernelMemoryAllocator.h>
#include <Kernel/Memory/MemoryManager.h>
#include <Kernel/Memory/PageTable.h>
#include <Kernel/Memory/PageTableEntry.h>
#include <Kernel/Process/ProcessManager.h>
#include <Kernel/Process/Scheduler.h>
#include <Kernel/System/DeviceManager.h>
#include <Kernel/System/System.h>
#include <Kernel/System/Timer.h>
#include <Utils/DebugConsole.h>
#include <Utils/Strings/String.h>

using Kernel::CPU;
using Kernel::DeviceManager;
using Kernel::ProcessManager;
using Kernel::SATP;
using Kernel::SatpMode;
using Kernel::Scheduler;
using Kernel::Timer;
using Kernel::TrapFrame;
using Kernel::Memory::EntryBits;
using Kernel::Memory::KernelMemoryAllocator;
using Kernel::Memory::MemoryManager;
using Kernel::Memory::Page;
using Kernel::Memory::PageTable;
using Kernel::Memory::PageTableEntry;
using Kernel::Memory::PhysicalAddress;
using Kernel::Memory::VirtualAddress;
using Utils::DebugConsole;
using Utils::ErrorOr;
using Utils::Strings::String;

PageTable* init_memory();
void sizeof_test();

extern "C" void kmain([[maybe_unused]] int a0, FDTHeader* header) {
  DebugConsole::println("RiscVOS: v0.0.1, U-Boot + OpenSBI, SPL configuration");
  auto* page_table = init_memory();
  System::init();
  KernelMemoryAllocator::the().debug();
  auto& system = System::the();
  DebugConsole::println("MemoryManager: Mapping the FDT.");
  MemoryManager::the().identity_map_range(*page_table, 0x8c000000, 0x8c000000 + *header->totalsize,
                                          (u64) EntryBits::READ_WRITE);
  DeviceManager::init();
  system.parse_fdt(header);
  sizeof_test();
  ProcessManager::init(page_table);
  auto* dummy_root = ProcessManager::the().create_dummy_process(MemoryManager::get_text_start(), MemoryManager::get_text_end());
  MemoryManager::the().identity_map_range(*dummy_root, MemoryManager::get_context_switching_start(), MemoryManager::get_context_switching_end(), (u64) Kernel::Memory::EntryBits::READ_EXECUTE);
  system.setup_interrupts();
  system.set_default_trap_vector();
  Timer::init();
  Scheduler::init();
  DebugConsole::println("Initialization completed");
  //DebugConsole::printf("Testing the printf function: {}\n", 42);
  Scheduler::the().start_scheduling();
}

void sizeof_test() {
  DebugConsole::print("Size of size_t: ");
  DebugConsole::print_ln_number(sizeof(size_t) * 8, 10);
  DebugConsole::print("Size of u64: ");
  DebugConsole::print_ln_number(sizeof(u64) * 8, 10);
  DebugConsole::print("Size of u32: ");
  DebugConsole::print_ln_number(sizeof(u32) * 8, 10);
  DebugConsole::print("Size of u8: ");
  DebugConsole::print_ln_number(sizeof(u8) * 8, 10);
  DebugConsole::print("Size of uintptr_t: ");
  DebugConsole::print_ln_number(sizeof(uintptr_t) * 8, 10);
}

PageTable* init_memory() {
  DebugConsole::println("Initializing memory.");
  MemoryManager::init();
  auto* page_table = (PageTable*) MemoryManager::the().alloc(1);

  MemoryManager::the().map_system_defaults(*page_table);

  DebugConsole::println("MemoryManager: Initial mapping done.");
  KernelMemoryAllocator::init(MemoryManager::the().zalloc(1));

  DebugConsole::println("System: Setting up SATP.");
  auto satp =
          Kernel::CPU::build_satp(Kernel::SatpMode::Sv39, 0, (uintptr_t) page_table);
  DebugConsole::println("System: Writing SATP.");
  CPU::write_satp(*(u64*) &satp);
  return page_table;
}
