#include <Kernel/Memory/PageTable.h>
#include <Kernel/Memory/PageTableEntry.h>
#include <Kernel/Memory/MemoryManager.h>
#include <Kernel/Memory/KernelMemoryAllocator.h>
#include <Kernel/Satp.hh>
#include <Kernel/CPU.hh>
#include <Utils/DebugConsole.hh>

using Kernel::Memory::Page;
using Kernel::Memory::EntryBits;
using Kernel::Memory::PageTableEntry;
using Kernel::Memory::MemoryManager;
using Kernel::Memory::KernelMemoryAllocator;
using Kernel::Memory::PageTable;
using Kernel::SatpMode;
using Kernel::CPU;
using Kernel::SATP;
using Kernel::Memory::VirtualAddress;
using Kernel::Memory::PhysicalAddress;
using Kernel::TrapFrame;
using Utils::DebugConsole;


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
}

PageTable* initMemory();

extern "C" void kmain(){
    {
        DebugConsole::println("RiscVOS: v0.0.1, U-Boot + OpenSBI, SPL configuration");
        auto* pageTable = initMemory();
        KernelMemoryAllocator::the().debug();
        for(;;){}
    }
}

PageTable* initMemory() {
    DebugConsole::println("Initializing memory.");
    MemoryManager::init((uintptr_t) &_heap_start, (uintptr_t) &_heap_size);
    auto& memoryManager = MemoryManager::the();
    auto* pageTable = (PageTable*) memoryManager.alloc(1);
    memoryManager.debugOutput();

    auto numPages = ((uintptr_t)(&_heap_size))/MemoryManager::PAGE_SIZE;
    DebugConsole::println("MemoryManager: Mapping the heap.");
    memoryManager.mapRange(*pageTable, (uintptr_t)(&_heap_start), ((uintptr_t) &_heap_start) + numPages, (u64)EntryBits::READ_WRITE);
    DebugConsole::println("MemoryManager: Mapping the text section.");
    memoryManager.mapRange(*pageTable, (uintptr_t) &_text_start, (uintptr_t) &_text_end, (u64)EntryBits::READ_EXECUTE);
    DebugConsole::println("MemoryManager: Mapping the rodata section.");
    memoryManager.mapRange(*pageTable, (uintptr_t) &_rodata_start, (uintptr_t) &_rodata_end, (u64)EntryBits::READ_EXECUTE);
    DebugConsole::println("MemoryManager: Mapping the data section.");
    memoryManager.mapRange(*pageTable, (uintptr_t) &_data_start, (uintptr_t) &_data_end, (u64)EntryBits::READ_WRITE);
    DebugConsole::println("MemoryManager: Mapping the bss section.");
    memoryManager.mapRange(*pageTable, (uintptr_t) &_bss_start, (uintptr_t) &_bss_end, (u64)EntryBits::READ_WRITE);
    DebugConsole::println("MemoryManager: Mapping the stack.");
    memoryManager.mapRange(*pageTable, (uintptr_t) &_stack_start, (uintptr_t) &_stack_end, (u64)EntryBits::READ_WRITE);
    DebugConsole::println("MemoryManager: Mapping the UART.");
    memoryManager.map(*pageTable, 0x10000000,  0x10000000, (u64)EntryBits::READ_WRITE, 0);
    DebugConsole::println("MemoryManager: Mapping the CLINT.");
    memoryManager.mapRange(*pageTable, 0x02000000, 0x0200ffff, (u64)EntryBits::READ_WRITE);
    DebugConsole::println("MemoryManager: Mapping the PLIC.");
    memoryManager.mapRange(*pageTable, 0x0c000000, 0x0c002001, (u64)EntryBits::READ_WRITE);
    memoryManager.mapRange(*pageTable, 0x0c200000, 0x0c208001, (u64)EntryBits::READ_WRITE);
    DebugConsole::println("MemoryManager: Initial mapping done.");
    KernelMemoryAllocator::init(memoryManager.zalloc(1));

    DebugConsole::println("Setting up SATP.");
    auto satp = Kernel::CPU::buildSatp(Kernel::SatpMode::Sv39, 0, (uintptr_t) pageTable);
    DebugConsole::println("Writing SATP.");
    CPU::writeSatp(*(u64*)&satp);
    return pageTable;
}



void initTraps() {

}

void initProcesses() {

}