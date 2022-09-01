#include <Kernel/Memory/PageTable.h>
#include <Kernel/Memory/PageTableEntry.h>
#include <Utils/Basic.hh>

namespace Kernel::Memory {
    void PageTable::debugOutput(){
        char buffer[64];
        for(int i = 0; i < 520; i++){
            PageTableEntry& entry = this->entries[i];
            u64 value = entry.getValue();
            if(value != 0){
                itoa(buffer, i, 10);
                DebugConsole::print("The value of ");
                DebugConsole::print(buffer);
                DebugConsole::print(". PTE is: ");
                itoa(buffer, value, 2);
                DebugConsole::println(buffer);
            }
        }
    }
};