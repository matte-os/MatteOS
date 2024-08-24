#define KERNEL 1

#include <Kernel/Arch/riscv64//CPU.h>
#include <Kernel/Arch/riscv64/Satp.h>
#include <Kernel/FileSystem/VirtualFileSystem.h>
#include <Kernel/Firmware/DeviceTree.h>
#include <Kernel/Firmware/FDT/FDT.h>
#include <Kernel/Memory/KernelMemoryAllocator.h>
#include <Kernel/Memory/MemoryManager.h>
#include <Kernel/Memory/PageTable.h>
#include <Kernel/Memory/PageTableEntry.h>
#include <Kernel/Process/ProcessManager.h>
#include <Kernel/Process/Scheduler.h>
#include <Kernel/Syscalls/SyscallManager.h>
#include <Kernel/Devices/DeviceManager.h>
#include <Kernel/Drivers/DriverManager.h>
#include <Kernel/System/InterruptManager.h>
#include <Kernel/System/System.h>
#include <Kernel/System/Timer.h>
#include <Utils/DebugConsole.h>
#include <Utils/Strings/String.h>

using Kernel::CPU;
using Kernel::DeviceManager;
using Kernel::DeviceTree;
using Kernel::DriverManager;
using Kernel::EntryBits;
using Kernel::InterruptManager;
using Kernel::KernelMemoryAllocator;
using Kernel::MemoryManager;
using Kernel::Page;
using Kernel::PageTable;
using Kernel::PageTableEntry;
using Kernel::PhysicalAddress;
using Kernel::ProcessManager;
using Kernel::SATP;
using Kernel::SatpMode;
using Kernel::Scheduler;
using Kernel::SyscallManager;
using Kernel::Timer;
using Kernel::TrapFrame;
using Kernel::VirtualAddress;
using Kernel::VirtualFileSystem;
using Utils::DebugConsole;
using Utils::ErrorOr;
using Utils::String;

PageTable* init_memory();
void sizeof_test();

extern "C" void kmain([[maybe_unused]] int a0, FDTHeader* header) {
  DebugConsole::println("RiscVOS: v0.0.1, U-Boot + OpenSBI, SPL configuration");
  auto* page_table = init_memory();
  System::init();

  //FIXME: We should have a trap handler setup here
  //so when we get a trap we can handle it/halt the system.
  KernelMemoryAllocator::the().debug();
  auto& system = System::the();
  system.set_default_trap_vector();

  DebugConsole::println("MemoryManager: Mapping the FDT.");
  MemoryManager::the().identity_map_range(*page_table, reinterpret_cast<uintptr_t>(header), reinterpret_cast<uintptr_t>(header) + *header->totalsize,
                                          (u64) EntryBits::READ_WRITE);

  DebugConsole::println("RiscVOS: Setting up VirtualFileSystem.");
  VirtualFileSystem::init();

  DebugConsole::println("RiscVOS: Setting up devices.");
  DeviceTree::init(header);
  DeviceManager::init();
  system.install_from_device_tree();
  DriverManager::init();

  DebugConsole::println("RiscVOS: Loading drivers.");
  DeviceManager::the().load_drivers();

  DebugConsole::println("RiscVOS: Block Device write test.");
  auto block_devices = DeviceManager::the().get_devices_of_type(Kernel::DeviceType::Block);
  for(size_t i = 0; i < block_devices.size(); i++) {
    auto device = block_devices[i];
    auto block_device = device->as<Kernel::BlockDevice>();
    auto buffer = new char[512];
    buffer[0] = 'H';
    buffer[1] = 'e';
    buffer[2] = 'l';
    buffer[3] = 'l';
    buffer[4] = 'o';
    auto write_result = block_device->write((u8*) buffer, 512, 0);
    if(write_result.has_error()) {
      DebugConsole::println("RiscVOS: Block Device write failed.");
    }
  }

  DebugConsole::println("RiscVOS: Setting up processes.");
  ProcessManager::init(page_table);
  auto* dummy_root = ProcessManager::the().create_dummy_process(MemoryManager::get_text_start(), MemoryManager::get_text_end());
  MemoryManager::the().identity_map_range(*dummy_root, MemoryManager::get_context_switching_start(), MemoryManager::get_context_switching_end(), (u64) Kernel::EntryBits::READ_EXECUTE);

  //TODO: Here we should try to load the init process from the rootfs

  DebugConsole::println("RiscVOS: Setting up interrupts.");
  InterruptManager::init();
  //TODO: Enable device (external) interrupts

  Timer::init();
  Scheduler::init();
  SyscallManager::init();
  DebugConsole::println("Initialization completed");

  //TODO: Here we should start the init process and the scheduler
  Scheduler::the().start_scheduling();
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
