#define KERNEL 1

#include <Kernel/CPU.h>
#include <Kernel/Firmware/FDT/fdt.h>
#include <Kernel/Memory/KernelMemoryAllocator.h>
#include <Kernel/Memory/MemoryManager.h>
#include <Kernel/Memory/PageTable.h>
#include <Kernel/Memory/PageTableEntry.h>
#include <Kernel/Process/ProcessManager.h>
#include <Kernel/Process/Scheduler.h>
#include <Kernel/Satp.h>
#include <Kernel/System/System.h>
#include <Utils/DebugConsole.h>
#include <Utils/Strings/String.h>

using Kernel::CPU;
using Kernel::SATP;
using Kernel::SatpMode;
using Kernel::TrapFrame;
using Kernel::Memory::EntryBits;
using Kernel::Memory::KernelMemoryAllocator;
using Kernel::Memory::MemoryManager;
using Kernel::Memory::Page;
using Kernel::Memory::PageTable;
using Kernel::Memory::PageTableEntry;
using Kernel::Memory::PhysicalAddress;
using Kernel::Memory::VirtualAddress;
using Kernel::Process::ProcessManager;
using Kernel::Process::Scheduler;
using Kernel::System::System;
using Utils::DebugConsole;
using Utils::Strings::String;

extern "C" {
char _data_start;
char _data_end;
char _rodata_start;
char _rodata_end;
char _text_start;
char _text_end;
char _bss_start;
char _bss_end;
char _memory_start;
char _memory_end;
char _heap_start;
char _heap_size;
char _stack_start;
char _stack_end;
char _context_switching_start;
char _context_switching_end;
char _text_special_start;
char _text_special_end;
}

PageTable *init_memory();
void sizeof_test();

extern "C" void kmain([[maybe_unused]] int a0, FDTHeader*header) {
  DebugConsole::println("RiscVOS: v0.0.1, U-Boot + OpenSBI, SPL configuration");
  auto *page_table = init_memory();
  System::init();
  KernelMemoryAllocator::the().debug();
  auto &system = System::the();
  // FIXME: This should be mapped exactly for the size of the FDT blob.
  DebugConsole::println("MemoryManager: Mapping the FDT.");
  MemoryManager::the().identity_map_range(*page_table, 0x8c000000, 0x8c000000 + *header->totalsize,
                                   (u64)EntryBits::READ_WRITE);
  system.parse_fdt(header);
  sizeof_test();
  DebugConsole::println("Tejd");
  ProcessManager::init(page_table);
  auto* dummy_root = ProcessManager::the().create_dummy_process(reinterpret_cast<uintptr_t>(&_text_start), reinterpret_cast<uintptr_t>(&_text_end));
  MemoryManager::the().identity_map_range(*dummy_root, (uintptr_t)&_context_switching_start, (uintptr_t)&_context_switching_end, (u64)Kernel::Memory::EntryBits::READ_EXECUTE);
  system.setup_interrupts();
  system.set_default_trap_vector();
  Scheduler::init();
  DebugConsole::println("Initialization completed");
  DebugConsole::printf("Testing the printf function: {}\n", 42);
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

PageTable *init_memory() {
  DebugConsole::println("Initializing memory.");
  MemoryManager::init((uintptr_t)&_heap_start, (uintptr_t)&_heap_size);
  auto &memoryManager = MemoryManager::the();
  auto *pageTable = (PageTable *)memoryManager.alloc(1);
  memoryManager.debug_output();

  DebugConsole::println("MemoryManager: Mapping the heap.");
  memoryManager.identity_map_range(*pageTable, (uintptr_t)(&_heap_start),
                          ((uintptr_t)((u64)&_heap_start + (u64)&_heap_size)),
                          (u64)EntryBits::READ_WRITE);
  DebugConsole::println("MemoryManager: Mapping the text section.");
  memoryManager.identity_map_range(*pageTable, (uintptr_t)&_text_start,
                          (uintptr_t)&_text_end, (u64)EntryBits::READ_EXECUTE);
  DebugConsole::println("MemoryManager: Mapping the rodata section.");
  memoryManager.identity_map_range(*pageTable, (uintptr_t)&_rodata_start,
                          (uintptr_t)&_rodata_end,
                          (u64)EntryBits::READ_EXECUTE);
  DebugConsole::println("MemoryManager: Mapping the data section.");
  memoryManager.identity_map_range(*pageTable, (uintptr_t)&_data_start,
                          (uintptr_t)&_data_end, (u64)EntryBits::READ_WRITE);
  DebugConsole::println("MemoryManager: Mapping the bss section.");
  memoryManager.identity_map_range(*pageTable, (uintptr_t)&_bss_start,
                          (uintptr_t)&_bss_end, (u64)EntryBits::READ_WRITE);
  DebugConsole::println("MemoryManager: Mapping the stack.");
  memoryManager.identity_map_range(*pageTable, (uintptr_t)&_stack_start,
                          (uintptr_t)&_stack_end, (u64)EntryBits::READ_WRITE);
  DebugConsole::println("MemoryManager: Mapping the UART.");
  memoryManager.map(*pageTable, 0x10000000, 0x10000000,
                    (u64)EntryBits::READ_WRITE, 0);
  DebugConsole::println("MemoryManager: Mapping the CLINT.");
  memoryManager.identity_map_range(*pageTable, 0x02000000, 0x0200ffff,
                          (u64)EntryBits::READ_WRITE);
  DebugConsole::println("MemoryManager: Mapping the PLIC.");
  memoryManager.identity_map_range(*pageTable, 0x0c000000, 0x0c002001,
                          (u64)EntryBits::READ_WRITE);
  memoryManager.identity_map_range(*pageTable, 0x0c200000, 0x0c208001,
                          (u64)EntryBits::READ_WRITE);
  memoryManager.identity_map_range(*pageTable, (uintptr_t)&_context_switching_start, (uintptr_t)&_context_switching_end, (u64)EntryBits::READ_EXECUTE);
  memoryManager.identity_map_range(*pageTable, (uintptr_t)&_text_special_start, (uintptr_t)&_text_special_end, (u64)EntryBits::READ_EXECUTE);

  DebugConsole::println("MemoryManager: Initial mapping done.");
  KernelMemoryAllocator::init(memoryManager.zalloc(1));

  DebugConsole::println("Setting up SATP.");
  auto satp =
      Kernel::CPU::build_satp(Kernel::SatpMode::Sv39, 0, (uintptr_t)pageTable);
  DebugConsole::println("Writing SATP.");
  CPU::write_satp(*(u64 *)&satp);
  DebugConsole::println("More pls.");
  return pageTable;
}
