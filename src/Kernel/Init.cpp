#define KERNEL 1

#include <Kernel/Arch/riscv64//CPU.h>
#include <Kernel/Arch/riscv64/Satp.h>
#include <Kernel/Devices/DeviceManager.h>
#include <Kernel/Drivers/DriverManager.h>
#include <Kernel/FileSystem/FileSystem.h>
#include <Kernel/FileSystem/VirtualFileSystem.h>
#include <Kernel/Firmware/DeviceTree.h>
#include <Kernel/Firmware/FDT/FDT.h>
#include <Kernel/Logger.h>
#include <Kernel/Memory/KernelMemoryAllocator.h>
#include <Kernel/Memory/MemoryManager.h>
#include <Kernel/Memory/PageTable.h>
#include <Kernel/Memory/PageTableEntry.h>
#include <Kernel/Process/ProcessManager.h>
#include <Kernel/Process/Scheduler.h>
#include <Kernel/Syscalls/SyscallManager.h>
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
using Kernel::Logger;
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
using Kernel::System;
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
    MemoryManager::the().identity_map_range(*page_table, reinterpret_cast<uintptr_t>(header),
                                            reinterpret_cast<uintptr_t>(header) + *header->totalsize,
                                            static_cast<u64>(EntryBits::READ_WRITE));

    DebugConsole::println("RiscVOS: Setting up VirtualFileSystem.");
    VirtualFileSystem::init();

    DebugConsole::println("RiscVOS: Setting up devices.");
    DeviceTree::init(header);
    DeviceManager::init();
    system.install_from_device_tree();

    DriverManager::init();
    DebugConsole::println("RiscVOS: Initializing drivers.");
    DriverManager::the().init_drivers();

    DebugConsole::println("RiscVOS: Loading the device drivers.");
    DeviceManager::the().load_drivers();

    auto block_devices = DeviceManager::the().get_devices_of_type(Kernel::DeviceType::Block);
    /*
    DebugConsole::println("RiscVOS: Block Device write test.");
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
    }*/

    if (block_devices.size() == 0) {
        DebugConsole::println("RiscVOS: No block devices found.");
    } else {
        DebugConsole::println("RiscVOS: Setting up the root filesystem.");
        if (const auto root_inode = VirtualFileSystem::the().device_load_filesystem(block_devices[0]); root_inode.
            has_error()) {
            DebugConsole::println("RiscVOS: Failed to load root filesystem.");
        } else {
            DebugConsole::println("RiscVOS: Mounting the root filesystem.");
            if (const auto error_or_mount = VirtualFileSystem::the().mount_root_fs(root_inode.get_value());
                error_or_mount.has_error()) {
                DebugConsole::println("RiscVOS: Unable to mount the root filesystem.");
            } else {
                DebugConsole::println("RiscVOS: Root filesystem loaded and mounted.");
            }
        }
    }

    DebugConsole::println("RiscVOS: Setting up processes.");
    ProcessManager::init(page_table);

    //TODO: Here we should try to load the init process from the rootfs
    // or load the in-memory init process

    ProcessManager::the().create_initial_processes();

    DebugConsole::println("RiscVOS: Setting up interrupts.");
    InterruptManager::init();
    InterruptManager::the().set_threshold(0);
    InterruptManager::the().enable_device_interrupts();

    Logger::init();
    Logger::the().try_console_lookup();
    dbgln("RiscVOS: Logger initialized.");
    DebugConsole::switch_to_device();

    Timer::init();
    Scheduler::init();
    SyscallManager::init();
    DebugConsole::println("Initialization completed");

    DebugConsole::print("RiscVOS: The state of SIE is ");
    DebugConsole::print_ln_number(CPU::read_sie(), 16);

    //TODO: Here we should start the init process and the scheduler
    Scheduler::the().start_scheduling();
}

PageTable* init_memory() {
    DebugConsole::println("Initializing memory.");
    MemoryManager::init();
    auto* page_table = reinterpret_cast<PageTable*>(MemoryManager::the().alloc(1));

    MemoryManager::the().map_system_defaults(*page_table);

    DebugConsole::println("MemoryManager: Initial mapping done.");
    KernelMemoryAllocator::init(MemoryManager::the().zalloc(1));

    DebugConsole::println("System: Setting up SATP.");
    auto satp =
            Kernel::CPU::build_satp(Kernel::SatpMode::Sv39, 0, reinterpret_cast<uintptr_t>(page_table));
    DebugConsole::println("System: Writing SATP.");
    CPU::write_satp(*reinterpret_cast<u64*>(&satp));
    return page_table;
}
