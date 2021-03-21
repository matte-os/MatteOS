#include <Kernel/Uart.hh>
#include <Kernel/Paging.hh>
#include <Kernel/PagingTable.hh>
#include <Kernel/VirtualMemory.hh>
#include <Utils/DebugConsole.hh>
#include <Utils/Basic.hh>

using Kernel::Uart;
using Kernel::Paging;
using Kernel::PagingTable;
using Kernel::VirtualMemory;
using Kernel::EntryBits;
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

void mapRange(PagingTable& root, size_t start, size_t end, __int64 bits){
    size_t memAddr = start & !(Kernel::PAGE_SIZE - 1);
    size_t numKbPages = (Kernel::alignValue(end, 12) - memAddr) / Kernel::PAGE_SIZE;


    for(int i = 0; i < numKbPages; i++){
        Kernel::map(root, memAddr, memAddr, bits, 0);
        memAddr += 1 << 12;
    }

}


//EntryPoint
extern "C" size_t kinit(){
    Uart uart(0x10000000);
    uart.init();
    DebugConsole::println("UART initialized!");

    Paging::init(&_heap_start, &_heap_size);
    DebugConsole::println("Paging initialized!");

    VirtualMemory::init();
    DebugConsole::println("Virtual Memory initialized!");

    auto rootPtr = VirtualMemory::getPagingTable();
    size_t root = (size_t) rootPtr;
    auto kHeapHead = (size_t) VirtualMemory::getHead();
    auto kNumAlloc = VirtualMemory::getNumAllocations();

    DebugConsole::println("Preparing for identity mapping!");

    mapRange(*rootPtr, kHeapHead, kHeapHead + kNumAlloc * 4096, EntryBits::READ_WRITE);
    DebugConsole::println("1. Done");
    auto numPages = _heap_size/Kernel::PAGE_SIZE;
    DebugConsole::println("2. Done");
    mapRange(*rootPtr, _heap_start, _heap_start + numPages, EntryBits::READ_WRITE);
    DebugConsole::println("3. Done");
    mapRange(*rootPtr, _text_start, _text_end, EntryBits::READ_EXECUTE);
    DebugConsole::println("4. Done");
    mapRange(*rootPtr,  _rodata_start, _rodata_end, EntryBits::READ_EXECUTE);
    DebugConsole::println("5. Done");
    mapRange(*rootPtr, _data_start, _data_end, EntryBits::READ_WRITE);
    DebugConsole::println("6. Done");
    mapRange(*rootPtr, _bss_start, _bss_end, EntryBits::READ_WRITE);
    DebugConsole::println("7. Done");
    mapRange(*rootPtr, _stack_start, _stack_end, EntryBits::READ_WRITE);
    DebugConsole::println("8. Done");
    Kernel::map(*rootPtr, 0x10000000, 0x10000000, EntryBits::READ_WRITE, 0);
    DebugConsole::println("9. Done");
    Kernel::map(*rootPtr, 0x02000000, 0x02000000, EntryBits::READ_WRITE, 0);
    DebugConsole::println("10. Done");
    Kernel::map(*rootPtr, 0x0200b000, 0x0200b000, EntryBits::READ_WRITE, 0);
    DebugConsole::println("11. Done");
    Kernel::map(*rootPtr, 0x0200c000, 0x0200c000, EntryBits::READ_WRITE, 0);
    DebugConsole::println("12. Done");
    Kernel::map(*rootPtr, 0x0c000000, 0x0c000000, EntryBits::READ_WRITE, 0);
    DebugConsole::println("13. Done");
    Kernel::map(*rootPtr, 0x0c200000, 0x0c200000, EntryBits::READ_WRITE, 0);
    DebugConsole::println("14. Done");

    DebugConsole::println("Identity mapping done!");
    return (root >> 12 | 8 << 60);
}

extern "C" void kmain(){
    

    DebugConsole::println("Welcome to WatermeloneOS (RISC-V)");
    for(;;){}
}