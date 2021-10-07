#include <Kernel/Uart.hh>
#include <Utils/DebugConsole.hh>
#include <Utils/Basic.hh>
#include <Kernel/Page.hh>
#include <Kernel/PageController.hh>
#include <Kernel/PageTableEntry.hh>
#include <Kernel/PageTable.hh>
#include <Kernel/CPU.hh>
#include <Kernel/TrapFrame.hh>
#include <Kernel/Plic.hh>

using Kernel::Uart;
using Kernel::Page;
using Kernel::PageController;
using Kernel::EntryBits;
using Kernel::PageTableEntry;
using Kernel::PageTable;
using Kernel::SatpMode;
using Kernel::CPU;
using Kernel::SATP;
using Kernel::VirtualAddress;
using Kernel::PhysicalAddress;
using Kernel::TrapFrame;
using Kernel::Plic;
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

extern "C" void kmain(){
    u64* mTimeCmp = (u64*) 0x02004000;
    u64* mTime = (u64*) 0x0200bff8;
    *mTimeCmp = *mTime + 10000000;
//
    //u64* v = (u64*) 0x0;
    //*v = 0;    
    DebugConsole::println("Welcome to WatermeloneOS (RISC-V)");

    DebugConsole::println("Setting up PLIC");
    
    Plic::setTreshold(0);
    Plic::enable(10);
    Plic::setPriority(10, 1);

    int a = 0;

    for(;;){
        a++;
    }
}

extern "C" void kinitHart(size_t hartId){
    CPU::mscratchWrite((uintptr_t)&CPU::KERNEL_TRAP_FRAME[hartId]);
    CPU::sscratchWrite(CPU::mscratchRead());
    CPU::KERNEL_TRAP_FRAME[hartId].hartId = hartId;
}

//EntryPoint
extern "C" u64 kinit(){
    Uart uart(0x10000000);
    uart.init();
    DebugConsole::println("UART initialized!");
    PageController::init((uintptr_t) &_heap_start, (uintptr_t) &_heap_size);

    size_t* ptr = PageController::alloc(5);

    //For first page table
    size_t* zPtr = PageController::zalloc(1);

    PageController::debugOutput();

    PageController::dealloc(ptr);

    PageController::debugOutput();

    PageTable* pageTable = (PageTable*) zPtr;
    //DebugConsole::println("Entries finished.");
    //DebugConsole::println("Tak asi nic");
    //pageTable->debugOutput();
    //PageTableEntry& entry = pageTable->entries[0];
    //entry.valid = 0b1;
    //entry.ppn0 = 0b110110110;
    //pageTable->debugOutput();
    SATP satp = CPU::buildSatp(SatpMode::Sv39, 0, (uintptr_t) pageTable);
    
    char buffer[64];
    itoa(buffer, (size_t)SatpMode::Sv39,16);
    DebugConsole::println(buffer);
    itoa(buffer, *(u64*)&satp, 16);
    DebugConsole::println(buffer);
    
    CPU::writeSatp(*(u64*)&satp);
    //size_t pageSize = sizeof(PageTableEntry);
    //char buffer[64];
    //itoa(buffer, pageSize, 10);
    //DebugConsole::println(buffer);

    //char buffer[64];
    //itoa(buffer, (unsigned long) &_heap_size, 10);
    //DebugConsole::println(buffer);

    //for(;;){}

    DebugConsole::println("Preparing for identity mapping!");

    //PageController::mapRange(*pageTable, kHeapHead, (uintptr_t) kHeapHead + kNumAlloc * 4096, (u64)EntryBits::READ_WRITE);
    //DebugConsole::println("1. Done");
    auto numPages = ((uintptr_t)(&_heap_size))/PageController::PAGE_SIZE;
    DebugConsole::println("1. Done");
    PageController::mapRange(*pageTable, (uintptr_t)(&_heap_start), ((uintptr_t) &_heap_start) + numPages, (u64)EntryBits::READ_WRITE);
    DebugConsole::println("2. Done");
    PageController::mapRange(*pageTable, (uintptr_t) &_text_start, (uintptr_t) &_text_end, (u64)EntryBits::READ_EXECUTE);
    DebugConsole::println("3. Done");
    PageController::mapRange(*pageTable, (uintptr_t) &_rodata_start, (uintptr_t) &_rodata_end, (u64)EntryBits::READ_EXECUTE);
    DebugConsole::println("4. Done");
    PageController::mapRange(*pageTable, (uintptr_t) &_data_start, (uintptr_t) &_data_end, (u64)EntryBits::READ_WRITE);
    DebugConsole::println("5. Done");
    PageController::mapRange(*pageTable, (uintptr_t) &_bss_start, (uintptr_t) &_bss_end, (u64)EntryBits::READ_WRITE);
    DebugConsole::println("6. Done");
    PageController::mapRange(*pageTable, (uintptr_t) &_stack_start, (uintptr_t) &_stack_end, (u64)EntryBits::READ_WRITE);
    DebugConsole::println("7. Done");
    PageController::map(*pageTable, 0x10000000,  0x10000000, (u64)EntryBits::READ_WRITE, 0);
    DebugConsole::println("8. Done");
    PageController::mapRange(*pageTable, 0x02000000, 0x0200ffff, (u64)EntryBits::READ_WRITE);
    DebugConsole::println("9. Done");
    PageController::mapRange(*pageTable, 0x0c000000, 0x0c002001, (u64)EntryBits::READ_WRITE);
    DebugConsole::println("10. Done");
    PageController::mapRange(*pageTable, 0x0c200000, 0x0c208001, (u64)EntryBits::READ_WRITE);
    DebugConsole::println("11. Done");

    pageTable->debugOutput();

    PageController::debugOutput();
    //DebugConsole::printLnNumber((uintptr_t) pageTable, 2);
    //DebugConsole::printLnNumber(satp.ppn, 2);
    //DebugConsole::println("Printing all entries: ");
    //for(auto& entry : ((PageTable*)(satp.ppn << 12))->entries){
    //    if(entry.getValue() != 0){
    //        DebugConsole::printLnNumber(entry.getValue(), 2);
    //    }
    //}
    //PageController::debugPageWalk(pageTable);

    //DebugConsole::printLnNumber((uintptr_t)&kmain, 16);

    //PageController::mapRange(*pageTable, (uintptr_t) &kmain, ((uintptr_t) &kmain) + 30, (u64)EntryBits::READ_WRITE_EXECUTE);

    CPU::mscratchWrite((uintptr_t)&CPU::KERNEL_TRAP_FRAME[0]);
    CPU::sscratchWrite(CPU::mscratchRead());
    CPU::KERNEL_TRAP_FRAME[0].satp = satp;
    CPU::KERNEL_TRAP_FRAME[0].trapStack = (u64*)(PageController::zalloc(1) + PageController::PAGE_SIZE);
    PageController::mapRange(*pageTable, (uintptr_t)(CPU::KERNEL_TRAP_FRAME[0].trapStack - PageController::PAGE_SIZE), (uintptr_t)(CPU::KERNEL_TRAP_FRAME[0].trapStack), (u64)EntryBits::READ_WRITE);
    PageController::mapRange(*pageTable, CPU::mscratchRead(), CPU::mscratchRead() + sizeof(TrapFrame), (u64) EntryBits::READ_WRITE);
    CPU::satpFenceAsid(0);


    return *(u64*)&satp;
}